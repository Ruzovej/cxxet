/*
  Copyright 2025 Lukáš Růžička

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the
  terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 3 of the License, or (at your option) any
  later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
  details.

  You should have received a copy of the GNU Lesser General Public License along
  with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#include <atomic>
#include <charconv>
#include <exception>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

#include <nlohmann/json.hpp>

#include "log.hxx"
#include "now.hxx"
#include "result_processor.hxx"
#include "str_utils.hxx"

int main(int const argc, char const *const *const argv) {
  try {
    auto const usage = [&]() {
      cxxet_pp::log(
          "usage: " + std::string{argv[0]} +
              " [-v|--verbose] [-h|--help] [-g|--git-commit-hash-file <file>]"
              " [-c|--compact] [-j|--jobs <N>]"
              " <<-o|--out|--out-json <output_file>"
              " <input_file_1> [<input_file_2> ...]>|<input_dir>>",
          true);
    };
    auto consume_arg = [&usage, argc{argc - 1}, argv{argv + 1}](
                           bool const require = false,
                           std::string_view const missing_error_context_desc =
                               "") mutable -> std::string_view {
      if (argc <= 0) {
        if (require) {
          usage();
          throw "missing argument (" + std::string{missing_error_context_desc} +
              ')';
        }
        return "";
      }
      auto const arg{*argv};
      --argc;
      ++argv;
      return arg;
    };

    std::string_view output;
    std::vector<std::string_view> rest;
    std::optional<std::filesystem::path> file_with_hash;
    rest.reserve(static_cast<std::size_t>(std::max(argc - 3, 1)));
    int json_indent{2};
    // anything negative means unbounded
    // 0 means num of cores
    // anything positive means that number of jobs
    int num_jobs{0};

    while (true) { // why are there no init-statements for `while` loops?!
      auto const arg{consume_arg()};
      if (arg.empty()) {
        break;
      }

      if (arg == "-v" || arg == "--verbose") {
        cxxet_pp::set_verbose(
            true); // ignore repetitions - use the latest value
      } else if (arg == "-h" || arg == "--help") {
        usage();
        return EXIT_SUCCESS;
      } else if (arg == "-o" || arg == "--out" || arg == "--out-json") {
        output =
            consume_arg(true, arg); // ignore repetitions - use the latest value
      } else if (arg == "-c" || arg == "--compact") {
        json_indent = -1;
      } else if (arg == "-g" || arg == "--git-commit-hash-file") {
        file_with_hash.emplace(consume_arg(true, arg));
        if (!std::filesystem::exists(*file_with_hash)) {
          throw "git commit hash file '" + file_with_hash->string() +
              "' does not exist";
        }
      } else if (arg == "-j" || arg == "--jobs") {
        auto const num_jobs_str{consume_arg(true, arg)};
        auto const [_, ec] = std::from_chars(num_jobs_str.cbegin(),
                                             num_jobs_str.cend(), num_jobs);
        if (ec != std::errc{}) {
          throw "invalid number of jobs '" + std::string{num_jobs_str} + '\'';
        }
      } else {
        rest.emplace_back(arg);
      }
    }

    if (rest.empty()) {
      throw "missing input file(s) or directory";
    } else if (output.empty() && rest.size() != 1) {
      throw "missing output file (when multiple input files are given)";
    }

    std::filesystem::path output_file;
    std::vector<std::filesystem::path> input_files;

    const auto adjust_commit_hash_file_path_if_empty =
        [&file_with_hash](std::filesystem::path &&path) {
          if (file_with_hash == std::nullopt) {
            file_with_hash.emplace(std::move(path));

            if (!std::filesystem::exists(*file_with_hash)) {
              file_with_hash.reset();
            }
          }
        };

    if (output.empty()) {
      output_file = rest.front();
      if (!std::filesystem::is_directory(output_file)) {
        throw "when output file is not specified, the single input must be a "
              "directory";
      }

      output_file /= "large.json";

      adjust_commit_hash_file_path_if_empty(
          std::filesystem::path{rest.front()} / "commit_hash.json");

      for (auto const &entry :
           std::filesystem::directory_iterator{rest.front()}) {
        input_files.emplace_back(entry.path());
      }
    } else {
      output_file = output;

      adjust_commit_hash_file_path_if_empty(output_file.parent_path() /
                                            "commit_hash.json");

      for (auto const &strv : rest) {
        input_files.emplace_back(strv);
      }
    }

    auto const t0{cxxet_pp::now()};
    auto benchmarks{nlohmann::json::array()};
    static constexpr std::string_view meta_file_suffix{"_meta.json"};
    if (num_jobs == 1) {
      for (auto const &entry_path : input_files) {
        if (cxxet_pp::ends_with(entry_path.string(), meta_file_suffix)) {
          cxxet_pp::log("\tProcessing " + entry_path.string() + " ...");
          auto const t00{cxxet_pp::now()};
          cxxet_pp::process_benchmark(benchmarks, entry_path);
          cxxet_pp::log_time_diff("\tProcessed " + entry_path.string(), t00,
                                  cxxet_pp::now());
        }
      }
    } else {
      struct alignas(std::hardware_destructive_interference_size) json_helper {
        nlohmann::json j{nlohmann::json::array()};
      };

      auto const num_input_files{input_files.size()};
      std::vector<json_helper> sub_results(num_input_files, json_helper{});

      std::atomic<unsigned> next_file_index{0};
      std::mutex log_mtx;
      auto const postprocessing_worker = [&](std::size_t const th_ind,
                                             std::size_t const th_num) {
        std::string msg{};

        do {
          auto const my_file_index{next_file_index.fetch_add(
              1, std::memory_order_relaxed)}; // adds 1, returns previous value

          if (num_input_files <= my_file_index) {
            break;
          }

          auto const &entry_path{input_files[my_file_index]};

          if (cxxet_pp::ends_with(entry_path.string(), meta_file_suffix)) {
            auto const t00{cxxet_pp::now()};
            cxxet_pp::process_benchmark(sub_results[my_file_index].j,
                                        entry_path);

            if (cxxet_pp::get_verbose()) {
              msg = "\tThread " + std::to_string(th_ind) + "/" +
                    std::to_string(th_num) + " processed " +
                    entry_path.string();
              auto const t01{cxxet_pp::now()};

              std::lock_guard lck{log_mtx};
              cxxet_pp::log_time_diff(msg, t00, t01);
            }
          }
        } while (true);
      };

      std::size_t const num_ths{
          num_jobs == 0 ? std::max(std::thread::hardware_concurrency(), 1u)
                        : (num_jobs < 0 ? num_input_files
                                        : static_cast<std::size_t>(num_jobs))};

      std::vector<std::thread> ths;
      ths.reserve(std::max(num_ths - 1, num_input_files));
      for (std::size_t i{1}; i < num_ths; ++i) {
        ths.emplace_back(postprocessing_worker, i + 1, num_ths);
      }
      postprocessing_worker(1, num_ths);
      for (auto &th : ths) {
        th.join();
      }

      for (auto &sub_result : sub_results) {
        for (auto &jj : sub_result.j) {
          benchmarks.emplace_back(std::move(jj));
        }
      }
    }
    auto const t1{cxxet_pp::now()};
    cxxet_pp::log_time_diff("Postprocessed benchmark results", t0, t1, true);

    auto sorted_benchmarks{nlohmann::json::array()};
    {
      auto const json_elem_cmp_tuple = [](nlohmann::json const &j) {
        auto const &jbp{j["benchmark_params"]};
        return std::tuple{j["benchmark_name"].get<std::string_view>(),
                          jbp["num_iters"].get<long long>(),
                          jbp["marker_after_iter"].get<long long>(),
                          jbp["cxxet_reserve_buffer"].get<long long>(),
                          jbp["num_threads"].get<long long>(),
                          jbp["used_lib"].get<std::string_view>(),
                          jbp["subtype"].get<std::string_view>(),
                          jbp["reps_max"].get<long long>(),
                          jbp["rep_no"].get<long long>()};
      };

      using cmp_tuple_t = decltype(json_elem_cmp_tuple(benchmarks.front()));

      std::vector<std::pair<cmp_tuple_t, nlohmann::json *>> sortable_benchmarks;
      sortable_benchmarks.reserve(benchmarks.size());

      for (auto &j : benchmarks) {
        sortable_benchmarks.emplace_back(json_elem_cmp_tuple(j), &j);
      }

      std::sort(sortable_benchmarks.begin(), sortable_benchmarks.end(),
                [](auto const &a, auto const &b) { return a.first < b.first; });

      for (auto &[_, jp] : sortable_benchmarks) {
        sorted_benchmarks.emplace_back(std::move(*jp));
      }
    }
    auto const t2{cxxet_pp::now()};
    cxxet_pp::log_time_diff("Sorted postprocessed benchmark results", t1, t2,
                            true);

    nlohmann::json result = {
        {"benchmarks", std::move(sorted_benchmarks)},
    };

    if (file_with_hash.has_value()) {
      result["context"]["cxxet_git_hash"] = nlohmann::json::parse(std::ifstream{
          *file_with_hash})["context"]["cxxet_git_hash"]
                                                .get<std::string>();
    }

    std::ofstream{output_file} << result.dump(json_indent);

    cxxet_pp::log_time_diff("Saved results into file " + output_file.string(),
                            t2, cxxet_pp::now(), true);

    return EXIT_SUCCESS;
  } catch (std::exception const &e) {
    cxxet_pp::log_error(
        "Failed (3) to postprocess \"large\" benchmark results: " +
        std::string{e.what()});
  } catch (std::string const &msg) {
    cxxet_pp::log_error(
        "Failed (2) to postprocess \"large\" benchmark results: " + msg);
  } catch (char const *const msg) {
    cxxet_pp::log_error(
        "Failed (1) to postprocess \"large\" benchmark results: " +
        std::string{msg});
  } catch (...) {
    cxxet_pp::log_error("Failed (0) to postprocess \"large\" benchmark "
                        "results: unknown exception");
  }
  return EXIT_FAILURE;
}

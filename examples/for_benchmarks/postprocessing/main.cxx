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

#include <exception>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
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
              " [-v|--verbose] [-h|--help] [-c|--compact] <-o|--out|--out-json "
              "<output_file> <input_file_1> [<input_file_2>] "
              "...|<input_dir>>",
          true);
    };
    auto consume_arg = [&usage, argc{argc - 1},
                        argv{argv + 1}](bool const require =
                                            false) mutable -> std::string_view {
      if (argc <= 0) {
        if (require) {
          usage();
          throw "missing argument";
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
    rest.reserve(static_cast<std::size_t>(std::max(argc - 3, 1)));
    int json_indent{2};

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
        output = consume_arg(true); // ignore repetitions - use the latest value
      } else if (arg == "-c" || arg == "--compact") {
        json_indent = -1;
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
    std::optional<std::filesystem::path> file_with_hash;
    std::vector<std::filesystem::path> input_files;

    if (output.empty()) {
      output_file = rest.front();
      if (!std::filesystem::is_directory(output_file)) {
        throw "when output file is not specified, the single input must be a "
              "directory";
      }

      output_file /= "large.json";

      file_with_hash.emplace(std::filesystem::path{rest.front()} /
                             "commit_hash.json");

      for (auto const &entry :
           std::filesystem::directory_iterator{rest.front()}) {
        input_files.emplace_back(entry.path());
      }
    } else {
      output_file = output;

      file_with_hash.emplace(output_file.parent_path() / "commit_hash.json");

      for (auto const &strv : rest) {
        input_files.emplace_back(strv);
      }
    }

    if (!std::filesystem::exists(*file_with_hash)) {
      file_with_hash.reset();
    }

    auto const t0{cxxet_pp::now()};
    auto benchmarks{nlohmann::json::array()};
    // TODO parallelize?!
    for (auto const &entry_path : input_files) {
      static constexpr std::string_view meta_file_suffix{"_meta.json"};
      if (cxxet_pp::ends_with(entry_path.string(), meta_file_suffix)) {
        cxxet_pp::log("\tProcessing " + entry_path.string() + " ...");
        auto const t00{cxxet_pp::now()};
        cxxet_pp::process_benchmark(benchmarks, entry_path);
        cxxet_pp::log_time_diff("\tProcessed " + entry_path.string(), t00,
                                cxxet_pp::now());
      }
    }
    auto const t1{cxxet_pp::now()};
    cxxet_pp::log_time_diff("Postprocessed benchmark results", t0, t1, true);

    auto sorted_benchmarks{nlohmann::json::array()};
    {
      auto const json_elem_cmp_tuple = [](nlohmann::json const &j) {
        auto const &jbp{j["benchmark_params"]};
        return std::tuple(j["benchmark_name"].get<std::string_view>(),
                          jbp["num_iters"].get<long long>(),
                          jbp["marker_after_iter"].get<long long>(),
                          jbp["cxxet_reserve_buffer"].get<long long>(),
                          jbp["num_threads"].get<long long>(),
                          jbp["used_lib"].get<std::string_view>(),
                          jbp["subtype"].get<std::string_view>(),
                          jbp["reps_max"].get<long long>(),
                          jbp["rep_no"].get<long long>());
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

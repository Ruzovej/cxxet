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

#include <cstdlib>

#include <exception>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <utility>

#include <nlohmann/json.hpp>

#include "log.hxx"
#include "now.hxx"
#include "str_utils.hxx"

namespace {

struct stats {
  double mean;
  double stddev;
  double min;
  double max;
  long long cnt;
  double p02;
  double p50; // median
  double p98;
};

stats compute_stats(std::vector<double> const &values) {
  if (values.empty()) {
    throw "cannot compute stats over empty vector";
  }

  auto const n{static_cast<double>(values.size())};

  double mean{0.0};
  for (auto const v : values) {
    mean += v;
  }
  mean /= n;

  double stddev{0.0};
  for (auto const v : values) {
    stddev += (v - mean) * (v - mean);
  }
  stddev = std::sqrt(stddev / n);

  auto sorted_values{values};
  std::sort(sorted_values.begin(), sorted_values.end());

  double const min{sorted_values.front()};
  double const max{sorted_values.back()};

  auto percentile = [&](double const p) {
    if ((p < 0.0) || (p > 100.0)) {
      throw "invalid percentile " + std::to_string(p);
    }
    if (p == 100.0) {
      return sorted_values.back();
    }
    auto const idx{static_cast<std::size_t>(p / 100.0 * n)};
    return sorted_values[idx];
  };

  return stats{mean,
               stddev,
               min,
               max,
               static_cast<long long>(n),
               percentile(2.0),
               percentile(50.0),
               percentile(98.0)};
}

struct val_unit {
  double value{0.0};
  std::string unit{"ns"};
};

std::map<std::string, val_unit>
process_benchmark_thread_perfs(nlohmann::json const &thread_perfs) {
  std::map<std::string, std::vector<double>> sub_results;

  for (auto const &tp : thread_perfs) {
    // there should be exactly one nonzero value:
    sub_results["global_flush"].emplace_back(
        tp["global_flush_ns"].get<double>());
    sub_results["global_flush_target"].emplace_back(
        tp["global_flush_target_ns"].get<double>());
    // potentialy more non-zero values for those:
    sub_results["markers_submission"].emplace_back(
        tp["markers_submission_ns"].get<double>());
    sub_results["set_thread_name"].emplace_back(
        tp["set_thread_name_ns"].get<double>());
    sub_results["thread_flush"].emplace_back(
        tp["thread_flush_ns"].get<double>());
    sub_results["thread_reserve"].emplace_back(
        tp["thread_reserve_ns"].get<double>());
  }

  auto const n{static_cast<long long>(thread_perfs.size())};
  std::map<std::string, val_unit> results;

  for (auto &[name, vals] : sub_results) {
    auto const computed_stats{compute_stats(vals)};
    if (!cxxet_pp::begins_with(name, "global_flush") && (n > 1)) {
      results[name + "_mean"] = {computed_stats.mean, "ns"};
      results[name + "_stddev"] = {computed_stats.stddev, "ns"};
      results[name + "_min"] = {computed_stats.min, "ns"};
      results[name + "_p02"] = {computed_stats.p02, "ns"};
      results[name + "_p50"] = {computed_stats.p50, "ns"};
      results[name + "_p98"] = {computed_stats.p98, "ns"};
      results[name + "_max"] = {computed_stats.max, "ns"};
    } else {
      results[name] = {computed_stats.max, "ns"};
    }
  }

  return results;
}

std::map<std::string, val_unit> process_benchmark_raw_results(
    std::string_view const /*benchmark_name*/,
    std::filesystem::path const & /*results_file_path*/,
    std::string_view const /*traced*/) {
  std::map<std::string, val_unit> result;

  return result;
}

void process_benchmark(nlohmann::json &target_array,
                       std::filesystem::path const &meta_file_path) {
  auto const meta_json{nlohmann::json::parse(std::ifstream{meta_file_path})};

  // TODO optimize those `get<...>` calls?! E.g. specialize them to directly
  // return e.g. `std::filesystem::path`, `std::string_view`, ...:
  auto const benchmark_name{
      meta_json["meta_info"]["benchmark_name"].get<std::string>()};
  auto const traced{meta_json["meta_info"]["traced"].get<std::string>()};

  std::filesystem::path const cxxet_results_filename{
      meta_json["meta_info"]["cxxet_results_filename"].get<std::string>()};

  if (bool const is_regular_file{
          std::filesystem::is_regular_file(cxxet_results_filename)};
      (traced == "cxxet") != is_regular_file) {
    throw '\'' + meta_file_path.string() +
        "' contains inconsistency (traced == " + traced +
        ", is_regular_file == " + std::to_string(is_regular_file) + ")";
  }

  auto const param_cxxet_reserve_buffer{
      meta_json["meta_info"]["cxxet_reserve_buffer"].get<long long>()};
  auto const param_marker_after_iter{
      meta_json["meta_info"]["marker_after_iter"].get<long long>()};
  auto const param_num_iters{
      meta_json["meta_info"]["num_iters"].get<long long>()};
  auto const param_num_threads{
      meta_json["meta_info"]["num_threads"].get<long long>()};

  auto const num_reps{meta_json["meta_info"]["repetitions"].get<long long>()};
  auto const rep{meta_json["meta_info"]["repetition_index"].get<long long>()};

  auto const benchmark_name_full{
      benchmark_name + "/ni_" + std::to_string(param_num_iters) + "/ma_" +
      std::to_string(param_marker_after_iter) + "/rb_" +
      std::to_string(param_cxxet_reserve_buffer) + "/th_" +
      std::to_string(param_num_threads)};

  auto const write_measurements =
      [&](std::map<std::string, val_unit> const &measurements) {
        for (const auto &[name, vu] : measurements) {
          auto const benchmark_name_full_final{benchmark_name_full + '/' +
                                               name + '/' + traced};
          target_array.push_back(
              {{"benchmark_name", benchmark_name},
               {"benchmark_name_full", benchmark_name_full_final},
               {"value", vu.value},
               {"unit", vu.unit},
               {"rep", rep},
               {"num_reps", num_reps}});
        }
      };

  write_measurements(process_benchmark_thread_perfs(meta_json["thread_perfs"]));
  if (traced != "bare") {
    write_measurements(process_benchmark_raw_results(
        benchmark_name, cxxet_results_filename, traced));
  }
}

} // namespace

int main(int argc, char const *const *argv) {
  try {
    if ((argc > 1) && ((std::string_view{argv[1]} == "--verbose") ||
                       (std::string_view{argv[1]} == "-v"))) {
      --argc;
      ++argv;
      cxxet_pp::set_verbose(true);
    }
    std::filesystem::path results_dir;
    if (argc > 1) {
      results_dir = argv[1];
    } else {
      throw "missing input directory argument";
    }
    if (!std::filesystem::is_directory(results_dir)) {
      throw "input path '" + results_dir.string() + "' is not a directory";
    }

    auto const t0{cxxet_pp::now()};

    nlohmann::json meta_info = {
        {"cxxet_git_hash",
         nlohmann::json::parse(std::ifstream{
             results_dir / "commit_hash.json"})["context"]["cxxet_git_hash"]
             .get<std::string>()},
    };

    nlohmann::json benchmarks = nlohmann::json::array();

    // TODO parallelize?!
    for (auto const &entry : std::filesystem::directory_iterator{results_dir}) {
      if (!entry.is_regular_file()) {
        continue;
      }

      static constexpr std::string_view meta_file_suffix{"_meta.json"};
      auto const entry_path{entry.path()};
      if (cxxet_pp::ends_with(entry_path.string(), meta_file_suffix)) {
        cxxet_pp::log("\tProcessing " + entry_path.string() + " ...");
        auto const t00{cxxet_pp::now()};
        process_benchmark(benchmarks, entry_path);
        cxxet_pp::log_time_diff("\tProcessed " + entry_path.string(), t00,
                                cxxet_pp::now());
      }
    }

    auto const t1{cxxet_pp::now()};
    cxxet_pp::log_time_diff("Postprocessed benchmark results", t0, t1, true);

    nlohmann::json result = {
        {"context", std::move(meta_info)},
        {"benchmarks", std::move(benchmarks)},
    };

    auto const target_file{results_dir / "large.json"};

    std::ofstream ofs{target_file};
    ofs << result.dump(2);

    cxxet_pp::log_time_diff("Saved results into file " + target_file.string(),
                            t1, cxxet_pp::now(), true);

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

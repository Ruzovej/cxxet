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

#include "result_processor.hxx"

#include <fstream>
#include <string>

#include <nlohmann/json.hpp>

#include "statistics.hxx"
#include "str_utils.hxx"

namespace cxxet_pp {

namespace {

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
    auto const computed_stats{cxxet_pp::compute_stats(vals)};
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

} // namespace

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

  auto num_ths_str{std::to_string(param_num_threads)};
  if (num_ths_str.size() == 1) {
    num_ths_str = "00" + num_ths_str;
  } else if (num_ths_str.size() == 2) {
    num_ths_str = '0' + num_ths_str;
  }

  auto const benchmark_name_full{
      benchmark_name + "/nit_" + std::to_string(param_num_iters) + "/mai_" +
      std::to_string(param_marker_after_iter) + "/crb_" +
      std::to_string(param_cxxet_reserve_buffer) + "/ths_" + num_ths_str};

  auto const write_measurements =
      [&](std::map<std::string, val_unit> const &measurements) {
        for (const auto &[name, vu] : measurements) {
          auto const benchmark_name_full_final{benchmark_name_full + '/' +
                                               name + '/' + traced};
          target_array.push_back({
              {"benchmark_name", benchmark_name},
              {"benchmark_name_full", benchmark_name_full_final},
              {"benchmark_params",
               {
                   {"cxxet_reserve_buffer", param_cxxet_reserve_buffer},
                   {"marker_after_iter", param_marker_after_iter},
                   {"num_iters", param_num_iters},
                   {"num_threads", param_num_threads},
                   {"rep_no", rep},
                   {"reps_max", num_reps},
                   {"subtype", name},
                   {"used_lib", traced},
               }},
              {"result", vu.value},
              {"unit", vu.unit},
          });
        }
      };

  write_measurements(process_benchmark_thread_perfs(meta_json["thread_perfs"]));
  if (traced != "bare") {
    write_measurements(process_benchmark_raw_results(
        benchmark_name, cxxet_results_filename, traced));
  }
}

} // namespace cxxet_pp

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
#include <map>
#include <numeric>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "statistics.hxx"
#include "str_utils.hxx"

namespace cxxet_pp {

namespace {

struct val_unit {
  double value{0.0};
  std::string_view unit{"ns"};
};

void write_val_unit_stats(std::map<std::string, val_unit> &target,
                          stats const &s, std::string const &name,
                          std::string_view const unit) {
  bool const all_percentiles{s.percentiles_near_min_max_meaningful()};
  target[name + "_mean"] = {s.mean, unit};
  target[name + "_stddev"] = {s.stddev, unit};
  target[name + "_min"] = {s.min, unit};
  if (all_percentiles) {
    target[name + "_p02"] = {s.p02, unit};
  }
  target[name + "_p25"] = {s.p25, unit};
  target[name + "_p50"] = {s.p50, unit};
  target[name + "_p75"] = {s.p75, unit};
  if (all_percentiles) {
    target[name + "_p98"] = {s.p98, unit};
  }
  target[name + "_max"] = {s.max, unit};
}

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

  std::map<std::string, val_unit> results;

  for (auto const &[name, vals] : sub_results) {
    auto const n{static_cast<long long>(vals.size())};
    if (!cxxet_pp::begins_with(name, "global_flush") && (n > 1)) {
      auto const computed_stats{cxxet_pp::compute_stats(vals)};
      write_val_unit_stats(results, computed_stats, name, "ns");
    } else if (n <= 0) {
      throw "missing `thread_perfs` data";
    } else {
      results[name] = {vals.front(), "ns"};
    }
  }

  return results;
}

// all `ts` & `dur` fields are in [us]
constexpr double double_us_to_ns(double const us) noexcept {
  return us * 1'000.0;
}

std::map<std::string, val_unit>
process_benchmark_raw_results(std::string_view const benchmark_name,
                              std::filesystem::path const &results_file_path,
                              std::string_view const traced) {
  std::map<std::string, val_unit> result;

  auto const require = [&](bool const cond, std::string const msg = "") {
    if (!cond) {
      throw "mandatory condition (" + msg + ") unsatisfied; benchmark '" +
          std::string{benchmark_name} + "', results file '" +
          results_file_path.string() + '\'';
    }
  };

  if (traced == "cxxet") {
    auto const results_json{
        nlohmann::json::parse(std::ifstream{results_file_path})};

    // should exhaust all "options"/targets in
    // `examples/for_benchmarks/CMakeLists.txt`:
    if (benchmark_name == "cxxet_bench_mt_counter") {
      std::map<long long, std::vector<double>> thread_timestamps;

      for (auto const &j : results_json["traceEvents"]) {
        require(j["ph"].get<std::string>() == "C", "phase value");
        require(j["name"].get<std::string>() == "Counter", "marker name");

        auto const tid{j["tid"].get<long long>()};
        auto const ts{j["ts"].get<double>()};

        thread_timestamps[tid].emplace_back(double_us_to_ns(ts));
      }

      std::size_t total_markers{0};
      for (auto &[tid, tss] : thread_timestamps) {
        require(tss.size() >= 2, "minimal amount of data");
        std::sort(tss.begin(), tss.end()); // just to be sure ...
        total_markers += tss.size() - 1;
      }

      std::vector<double> diffs, tmp;
      diffs.reserve(total_markers);
      for (auto const &[tid, tss] : thread_timestamps) {
        tmp.clear();
        tmp.reserve(tss.size());

        std::adjacent_difference(tss.cbegin(), tss.cend(),
                                 std::back_inserter(tmp));

        require(tmp.size() == tss.size(), "tmp diffs size");

        // first element is meaningless
        diffs.insert(diffs.end(), tmp.cbegin() + 1, tmp.cend());
      }
      require(diffs.size() == total_markers, "diffs size");
      std::sort(diffs.begin(), diffs.end());

      write_val_unit_stats(result, cxxet_pp::compute_stats(diffs, false),
                           "TRACE_counter_marker_interval", "ns");
    } else if (benchmark_name == "cxxet_bench_st_instant") {
      std::vector<double> timestamps;

      timestamps.reserve(results_json["traceEvents"].size());
      for (auto const &j : results_json["traceEvents"]) {
        require(j["ph"].get<std::string>() == "i", "phase value");
        require(j["name"].get<std::string>() == "some instant ...",
                "marker name");

        auto const ts{j["ts"].get<double>()};
        timestamps.emplace_back(double_us_to_ns(ts));
      }

      require(timestamps.size() >= 2, "minimal amount of data");
      std::sort(timestamps.begin(), timestamps.end());

      std::vector<double> diffs;
      diffs.reserve(timestamps.size());
      std::adjacent_difference(timestamps.cbegin(), timestamps.cend(),
                               std::back_inserter(diffs));

      require(diffs.size() == timestamps.size(), "diffs size");

      // first element is meaningless
      diffs.erase(diffs.begin());
      std::sort(diffs.begin(), diffs.end());

      write_val_unit_stats(result, cxxet_pp::compute_stats(diffs, false),
                           "TRACE_instant_marker_interval", "ns");
    } else if (benchmark_name == "cxxet_bench_st_guarded_instant") {
      auto const num_events{results_json["traceEvents"].size()};
      require(num_events % 2 == 0, "even number of events");
      require(num_events > 0, "some data");
      auto const num_event_pairs{num_events / 2};

      std::vector<double> guarding_marker_complete_lengths;
      guarding_marker_complete_lengths.reserve(num_event_pairs);
      std::vector<double> guarding_marker_complete_gaps;
      guarding_marker_complete_gaps.reserve(num_event_pairs - 1);
      std::vector<double> guarded_marker_instant_dist_from_complete_center;
      guarded_marker_instant_dist_from_complete_center.reserve(num_event_pairs);

      auto const &js{results_json["traceEvents"]};
      std::optional<double> prev_complete_end{};
      for (auto it{js.cbegin()}; it != js.cend(); it += 2) {
        auto const &j_inst{*it};
        require(j_inst["ph"].get<std::string>() == "i", "phase value");
        require(j_inst["name"].get<std::string>() == "some instant ...",
                "instant marker name");

        auto const mark_inst_ts{double_us_to_ns(j_inst["ts"].get<double>())};

        auto const &j_comp{*(it + 1)};
        require(j_comp["ph"].get<std::string>() == "X", "phase value");
        require(
            (j_comp["name"].get<std::string>() ==
             "complete over instant event ") // TODO delete this later ...
                                             // after adjusting/fixing the
                                             // benchmark itself
                ||
                (j_comp["name"].get<std::string>() ==
                 "complete over instant event") // NOTE: desired condition ...
            ,
            "complete marker name");

        auto const mark_comp_beg{double_us_to_ns(j_comp["ts"].get<double>())};
        auto const mark_comp_dur{double_us_to_ns(j_comp["dur"].get<double>())};
        auto const mark_comp_end{mark_comp_beg + mark_comp_dur};

        require(mark_comp_beg <= mark_inst_ts, "instant after complete begin");
        require(mark_inst_ts <= mark_comp_end, "instant before complete end");

        guarding_marker_complete_lengths.emplace_back(mark_comp_dur);
        if (prev_complete_end.has_value()) {
          guarding_marker_complete_gaps.emplace_back(mark_comp_beg -
                                                     prev_complete_end.value());
        }
        prev_complete_end.emplace(mark_comp_end);

        auto const mark_comp_center{mark_comp_beg + 0.5 * mark_comp_dur};
        guarded_marker_instant_dist_from_complete_center.emplace_back(
            mark_inst_ts - mark_comp_center);
      }

      require(guarding_marker_complete_lengths.size() == num_event_pairs,
              "marker_complete_lengths size");
      require(guarding_marker_complete_gaps.size() == num_event_pairs - 1,
              "marker_complete_gaps size");
      require(guarded_marker_instant_dist_from_complete_center.size() ==
                  num_event_pairs,
              "marker_instant_dist_from_complete_center size");

      std::sort(guarding_marker_complete_lengths.begin(),
                guarding_marker_complete_lengths.end());
      std::sort(guarding_marker_complete_gaps.begin(),
                guarding_marker_complete_gaps.end());
      std::sort(guarded_marker_instant_dist_from_complete_center.begin(),
                guarded_marker_instant_dist_from_complete_center.end());

      write_val_unit_stats(
          result,
          cxxet_pp::compute_stats(guarding_marker_complete_lengths, false),
          "TRACE_guarding_complete_marker_duration", "ns");
      write_val_unit_stats(
          result, cxxet_pp::compute_stats(guarding_marker_complete_gaps, false),
          "TRACE_guarding_complete_marker_gap", "ns");
      write_val_unit_stats(
          result,
          cxxet_pp::compute_stats(
              guarded_marker_instant_dist_from_complete_center, false),
          "TRACE_guarded_instant_dist_from_complete_center", "ns");
    } else if (benchmark_name == "cxxet_bench_st_complete") {
      auto const num_events{results_json["traceEvents"].size()};
      require(num_events > 0, "some data");

      std::vector<double> marker_complete_lengths;
      marker_complete_lengths.reserve(num_events);
      std::vector<double> marker_complete_gaps;
      marker_complete_gaps.reserve(num_events - 1);

      std::optional<double> prev_complete_end{};
      for (auto const &j_comp : results_json["traceEvents"]) {
        require(j_comp["ph"].get<std::string>() == "X", "phase value");
        require((j_comp["name"].get<std::string>() == "complete ..."),
                "complete marker name");

        auto const mark_comp_beg{double_us_to_ns(j_comp["ts"].get<double>())};
        auto const mark_comp_dur{double_us_to_ns(j_comp["dur"].get<double>())};
        auto const mark_comp_end{mark_comp_beg + mark_comp_dur};

        marker_complete_lengths.emplace_back(mark_comp_dur);
        if (prev_complete_end.has_value()) {
          marker_complete_gaps.emplace_back(mark_comp_beg -
                                            prev_complete_end.value());
        }
        prev_complete_end.emplace(mark_comp_end);
      }

      require(marker_complete_lengths.size() == num_events,
              "marker_complete_lengths size");
      require(marker_complete_gaps.size() == num_events - 1,
              "marker_complete_gaps size");

      std::sort(marker_complete_lengths.begin(), marker_complete_lengths.end());
      std::sort(marker_complete_gaps.begin(), marker_complete_gaps.end());

      write_val_unit_stats(
          result, cxxet_pp::compute_stats(marker_complete_lengths, false),
          "TRACE_complete_marker_duration", "ns");
      write_val_unit_stats(result,
                           cxxet_pp::compute_stats(marker_complete_gaps, false),
                           "TRACE_complete_marker_gap", "ns");
    } else if (benchmark_name == "cxxet_bench_st_duration") {
    } else {
      throw "unknown benchmark name '" + std::string{benchmark_name} + "'";
    }
  }

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
      [&](std::map<std::string, val_unit> const &measurements,
          std::filesystem::path const &source_file) {
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
              {"file_path_source", source_file},
              {"result", vu.value},
              {"unit", vu.unit},
          });
        }
      };

  write_measurements(process_benchmark_thread_perfs(meta_json["thread_perfs"]),
                     meta_file_path);
  if (traced != "bare") {
    write_measurements(process_benchmark_raw_results(
                           benchmark_name, cxxet_results_filename, traced),
                       cxxet_results_filename);
  }
}

} // namespace cxxet_pp

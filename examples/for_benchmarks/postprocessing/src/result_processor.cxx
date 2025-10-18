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
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "statistics.hxx"
#include "str_utils.hxx"

namespace cxxet_pp {

namespace {

constexpr std::string_view default_time_units{"ns"};

// all `ts` & `dur` fields are in [us]
constexpr double double_us_to_ns(double const us) noexcept {
  return us * 1'000.0;
}

struct val_unit {
  double value{0.0};
  std::string_view unit{};
};

void write_val_unit_stats(std::map<std::string, val_unit> &target,
                          stats const &computed_stats,
                          std::string const &name_base,
                          std::string_view const unit = default_time_units) {
  bool const all_percentiles{
      computed_stats.percentiles_near_min_max_meaningful()};
  target[name_base + "_mean"] = {computed_stats.mean, unit};
  target[name_base + "_stddev"] = {computed_stats.stddev, unit};
  target[name_base + "_min"] = {computed_stats.min, unit};
  if (all_percentiles) {
    target[name_base + "_p02"] = {computed_stats.p02, unit};
  }
  target[name_base + "_p25"] = {computed_stats.p25, unit};
  target[name_base + "_p50"] = {computed_stats.p50, unit};
  target[name_base + "_p75"] = {computed_stats.p75, unit};
  if (all_percentiles) {
    target[name_base + "_p98"] = {computed_stats.p98, unit};
  }
  target[name_base + "_max"] = {computed_stats.max, unit};
}

void write_val_unit_stats(std::map<std::string, val_unit> &target,
                          std::vector<double> &&data,
                          std::string_view const val_name_base,
                          std::string_view const unit = default_time_units) {
  write_val_unit_stats(target, cxxet_pp::stats::compute_from(std::move(data)),
                       std::string{val_name_base}, unit);
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

  for (auto &&[name, vals] : sub_results) {
    auto const n{static_cast<long long>(vals.size())};
    if (!cxxet_pp::begins_with(name, "global_flush") && (n > 1)) {
      write_val_unit_stats(results, std::move(vals), name, default_time_units);
    } else if (n <= 0) {
      throw "missing `thread_perfs` data";
    } else {
      results[name] = {vals.front(), default_time_units};
    }
  }

  return results;
}

struct value_diffs_collector {
  explicit value_diffs_collector(std::size_t const num_values = 0) {
    diffs.reserve(num_values);
  }

  template <typename ts_monotonicity_check_fn_t>
  void add_value(
      double const ts, ts_monotonicity_check_fn_t const &check_fn,
      std::string_view const check_ctx,
      std::optional<double> const adjusted_future_prev_ts = std::nullopt) {
    if (prev_ts.has_value()) {
      check_fn(prev_ts.value() <= ts, check_ctx);
      diffs.emplace_back(ts - prev_ts.value());
    }
    prev_ts.emplace(adjusted_future_prev_ts.value_or(ts));
  }

  std::vector<double> const &get_diffs() const & noexcept { return diffs; }
  std::vector<double> get_diffs() && noexcept { return std::move(diffs); }

private:
  std::optional<double> prev_ts{};
  std::vector<double> diffs;
};

std::map<std::string, val_unit>
process_benchmark_raw_results(std::string_view const benchmark_name,
                              std::filesystem::path const &results_file_path,
                              std::string_view const traced) {
  std::map<std::string, val_unit> result;

  auto const require = [&](bool const cond, std::string_view const msg = "") {
    if (!cond) {
      throw "mandatory condition (" + std::string{msg} +
          ") unsatisfied; benchmark '" + std::string{benchmark_name} +
          "', results file '" + results_file_path.string() + '\'';
    }
  };

  if (traced == "cxxet") {
    auto const results_json{
        nlohmann::json::parse(std::ifstream{results_file_path})};

    // should exhaust all "options"/targets in
    // `examples/for_benchmarks/CMakeLists.txt`:
    if (benchmark_name == "cxxet_bench_mt_counter") {
      std::map<long long, value_diffs_collector> thread_timestamps;

      for (auto const &j : results_json["traceEvents"]) {
        require(j["ph"].get<std::string>() == "C", "phase value");
        require(j["name"].get<std::string>() == "Counter", "marker name");

        auto const tid{j["tid"].get<long long>()};
        auto const ts{j["ts"].get<double>()};

        thread_timestamps[tid].add_value(double_us_to_ns(ts), require,
                                         "monotonic timestamps");
      }

      std::size_t total_marker_gaps{0};
      for (auto &[tid, tss] : thread_timestamps) {
        require(!tss.get_diffs().empty(), "some data per thread");
        total_marker_gaps += tss.get_diffs().size();
      }

      std::vector<double> diffs;
      diffs.reserve(total_marker_gaps);
      for (auto const &[tid, tss] : thread_timestamps) {
        diffs.insert(diffs.end(), tss.get_diffs().cbegin(),
                     tss.get_diffs().cend());
      }
      require(diffs.size() == total_marker_gaps, "all diffs size");

      thread_timestamps.clear(); // sligthly dangerous (during later
                                 // development, if overlooked & needed), but
                                 // speculatively save some memory here

      write_val_unit_stats(result, std::move(diffs),
                           "TRACE_counter_marker_interval");
    } else if (benchmark_name == "cxxet_bench_st_instant") {
      require(results_json["traceEvents"].size() >= 2, "some data");

      auto const expected_size{results_json["traceEvents"].size() - 1};

      value_diffs_collector diffs{expected_size};
      for (auto const &j : results_json["traceEvents"]) {
        require(j["ph"].get<std::string>() == "i", "phase value");
        require(j["name"].get<std::string>() == "some instant ...",
                "marker name");

        auto const ts{double_us_to_ns(j["ts"].get<double>())};

        diffs.add_value(ts, require, "monotonic timestamps");
      }
      require(diffs.get_diffs().size() == expected_size, "diffs size");

      write_val_unit_stats(result, std::move(diffs).get_diffs(),
                           "TRACE_instant_marker_interval");
    } else if (benchmark_name == "cxxet_bench_st_guarded_instant") {
      auto const num_events{results_json["traceEvents"].size()};
      require(num_events > 0, "some data");
      require(num_events % 2 == 0, "even number of events");
      auto const num_event_pairs{num_events / 2};
      auto const num_event_gaps{num_event_pairs - 1};

      std::vector<double> guarding_marker_complete_lengths;
      guarding_marker_complete_lengths.reserve(num_event_pairs);
      value_diffs_collector guarding_marker_complete_gaps{num_event_gaps};
      std::vector<double> guarded_marker_instant_dist_from_complete_center;
      guarded_marker_instant_dist_from_complete_center.reserve(num_event_pairs);

      auto const &js{results_json["traceEvents"]};
      for (auto it{js.cbegin()}, it_end = js.cend(); it != it_end; it += 2) {
        auto const &j_inst{*it};
        require(j_inst["ph"].get<std::string>() == "i", "phase value");
        require(j_inst["name"].get<std::string>() == "some instant ...",
                "instant marker name");

        auto const mark_inst_ts{double_us_to_ns(j_inst["ts"].get<double>())};

        auto const &j_comp{*(it + 1)};
        require(j_comp["ph"].get<std::string>() == "X", "phase value");
        require(j_comp["name"].get<std::string>() ==
                    "complete over instant event",
                "complete marker name");

        auto const mark_comp_beg{double_us_to_ns(j_comp["ts"].get<double>())};
        auto const mark_comp_dur{double_us_to_ns(j_comp["dur"].get<double>())};
        auto const mark_comp_end{mark_comp_beg + mark_comp_dur};

        require(mark_comp_beg <= mark_inst_ts, "instant after complete begin");
        require(mark_inst_ts <= mark_comp_end, "instant before complete end");

        auto const mark_comp_center{mark_comp_beg + 0.5 * mark_comp_dur};

        guarding_marker_complete_lengths.emplace_back(mark_comp_dur);
        guarding_marker_complete_gaps.add_value(
            mark_comp_beg, require, "monotonic timestamps", mark_comp_end);
        guarded_marker_instant_dist_from_complete_center.emplace_back(
            mark_inst_ts - mark_comp_center);
      }

      require(guarding_marker_complete_lengths.size() == num_event_pairs,
              "marker_complete_lengths size");
      write_val_unit_stats(result, std::move(guarding_marker_complete_lengths),
                           "TRACE_guarding_complete_marker_duration");

      require(guarding_marker_complete_gaps.get_diffs().size() ==
                  num_event_gaps,
              "marker_complete_gaps size");
      write_val_unit_stats(result,
                           std::move(guarding_marker_complete_gaps).get_diffs(),
                           "TRACE_guarding_complete_marker_gap");

      require(guarded_marker_instant_dist_from_complete_center.size() ==
                  num_event_pairs,
              "marker_instant_dist_from_complete_center size");
      write_val_unit_stats(
          result, std::move(guarded_marker_instant_dist_from_complete_center),
          "TRACE_guarded_instant_dist_from_complete_center");

    } else if (benchmark_name == "cxxet_bench_st_complete") {
      auto const num_events{results_json["traceEvents"].size()};
      require(num_events > 1, "some data");
      auto const num_gaps{num_events - 1};

      std::vector<double> marker_complete_lengths;
      marker_complete_lengths.reserve(num_events);
      value_diffs_collector marker_complete_gaps{num_gaps};

      for (auto const &j_comp : results_json["traceEvents"]) {
        require(j_comp["ph"].get<std::string>() == "X", "phase value");
        require((j_comp["name"].get<std::string>() == "complete ..."),
                "complete marker name");

        auto const mark_comp_beg{double_us_to_ns(j_comp["ts"].get<double>())};
        auto const mark_comp_dur{double_us_to_ns(j_comp["dur"].get<double>())};
        auto const mark_comp_end{mark_comp_beg + mark_comp_dur};

        marker_complete_lengths.emplace_back(mark_comp_dur);
        marker_complete_gaps.add_value(mark_comp_beg, require,
                                       "monotonic timestamps", mark_comp_end);
      }

      require(marker_complete_lengths.size() == num_events,
              "marker_complete_lengths size");
      write_val_unit_stats(result, std::move(marker_complete_lengths),
                           "TRACE_complete_marker_duration");
      require(marker_complete_gaps.get_diffs().size() == num_gaps,
              "marker_complete_gaps size");
      write_val_unit_stats(result, std::move(marker_complete_gaps).get_diffs(),
                           "TRACE_complete_marker_gap");
    } else if (benchmark_name == "cxxet_bench_st_duration") {
      auto const num_events{results_json["traceEvents"].size()};
      require(num_events > 0, "some data");
      require(num_events % 2 == 0, "even number of events");
      auto const num_event_pairs{num_events / 2};
      auto const num_gaps{num_event_pairs - 1};

      std::vector<double> marker_duration_lengths;
      marker_duration_lengths.reserve(num_event_pairs);
      value_diffs_collector marker_duration_gaps{num_gaps};

      auto const &js{results_json["traceEvents"]};
      for (auto it{js.cbegin()}, it_end = js.cend(); it != it_end; it += 2) {
        auto const &j_begin{*it};
        require(j_begin["ph"].get<std::string>() == "B", "phase value");
        require(j_begin["name"].get<std::string>() == "begin marker ...",
                "begin marker name");

        auto const mark_begin_ts{double_us_to_ns(j_begin["ts"].get<double>())};

        auto const &j_end{*(it + 1)};
        require(j_end["ph"].get<std::string>() == "E", "phase value");

        auto const mark_end_ts{double_us_to_ns(j_end["ts"].get<double>())};

        require(mark_begin_ts <= mark_end_ts, "begin before end");

        marker_duration_lengths.emplace_back(mark_end_ts - mark_begin_ts);
        marker_duration_gaps.add_value(mark_begin_ts, require,
                                       "monotonic timestamps", mark_end_ts);
      }

      require(marker_duration_lengths.size() == num_event_pairs,
              "marker_duration_lengths size");
      write_val_unit_stats(result, std::move(marker_duration_lengths),
                           "TRACE_duration_marker_duration");
      require(marker_duration_gaps.get_diffs().size() == num_event_pairs - 1,
              "marker_duration_gaps size");
      write_val_unit_stats(result, std::move(marker_duration_gaps).get_diffs(),
                           "TRACE_duration_marker_gap");
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

  auto const write_measurements =
      [&](std::map<std::string, val_unit> const &measurements,
          std::filesystem::path const &source_file) {
        for (const auto &[name, vu] : measurements) {
          target_array.push_back({
              {"benchmark_name", benchmark_name},
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

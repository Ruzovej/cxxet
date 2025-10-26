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

#include "compare.hxx"

#include <array>
#include <fstream>
#include <map>
#include <string_view>
#include <vector>

#include <nlohmann/json.hpp>

#include "log.hxx"
#include "mann_whitney_u_test.hxx"
#include "statistics.hxx"

namespace cxxet_cmp {

namespace {

struct measurement {
  explicit measurement(nlohmann::json const &j)
      : benchmark_name{j["benchmark_name"].get<std::string_view>()},
        cxxet_reserve_buffer{
            j["benchmark_params"]["cxxet_reserve_buffer"].get<int>()},
        marker_after_iter{
            j["benchmark_params"]["marker_after_iter"].get<int>()},
        num_iters{j["benchmark_params"]["num_iters"].get<int>()},
        num_threads{j["benchmark_params"]["num_threads"].get<int>()},
        subtype{j["benchmark_params"]["subtype"].get<std::string_view>()},
        used_lib{j["benchmark_params"]["used_lib"].get<std::string_view>()} {}

  std::string_view benchmark_name;
  int cxxet_reserve_buffer;
  int marker_after_iter;
  int num_iters;
  int num_threads;
  std::string_view subtype;
  std::string_view used_lib;

  bool operator<(measurement const &other) const noexcept {
    auto const tpl = [](measurement const &m) {
      return std::tuple{
          m.benchmark_name, m.cxxet_reserve_buffer, m.marker_after_iter,
          m.num_iters,      m.num_threads,          m.subtype,
          //  m.used_lib // intentionally left out of comparison ...
      };
    };
    return tpl(*this) < tpl(other);
  }

  std::string to_string() const {
    return std::string{benchmark_name} +
           " (crb=" + std::to_string(cxxet_reserve_buffer) +
           ", mai=" + std::to_string(marker_after_iter) +
           ", nit=" + std::to_string(num_iters) +
           ", nths=" + std::to_string(num_threads) +
           ", subtype=" + std::string{subtype} +
           ", impl=" + std::string{used_lib} + ')';
  }
};

enum class kind_e : unsigned { k_baseline = 0, k_challenger = 1 };

constexpr unsigned get_index(kind_e const k) noexcept {
  return static_cast<unsigned>(k);
}

constexpr unsigned baseline_ind{get_index(kind_e::k_baseline)};
constexpr unsigned challenger_ind{get_index(kind_e::k_challenger)};

template <kind_e kind>
void extract_measurements(
    std::map<measurement, std::array<std::vector<double>, 2>> &results,
    nlohmann::json const &j, std::string_view const required_used_lib) {
  for (auto const &elem : j["benchmarks"]) {
    if (elem["benchmark_params"]["used_lib"].get<std::string_view>() ==
        required_used_lib) {
      auto const value{elem["result"].get<double>()};

      results[measurement{elem}][get_index(kind)].emplace_back(value);
    }
  }
}

} // namespace

void compare_files(std::filesystem::path const &baseline,
                   std::filesystem::path const &challenger,
                   std::filesystem::path const &json_output,
                   int const json_indent) {

  static constexpr std::string_view p_eq{"probably_equivalent"};
  static constexpr std::string_view p_chall_better{
      "probably_challenger_better"};
  static constexpr std::string_view p_chall_worse{"probably_challenger_worse"};
  static constexpr std::string_view chall_miss{"challenger_missing"};
  static constexpr std::string_view base_miss{"baseline_missing"};

  nlohmann::json results_json;
  if (!json_output.empty()) {
    results_json = {
        {"comparing",
         {
             {"mode", "TODO"},
             {"inputs", nlohmann::json::array()},
         }},
        {"total", {}},
        {"comparison_results",
         {
             {p_eq, nlohmann::json::array()},
             {p_chall_better, nlohmann::json::array()},
             {p_chall_worse, nlohmann::json::array()},
             {chall_miss, nlohmann::json::array()},
             {base_miss, nlohmann::json::array()},
         }},
    };
  }

  std::map<measurement, std::array<std::vector<double>, 2>> results;

  nlohmann::json const baseline_json{
      nlohmann::json::parse(std::ifstream{baseline})};
  if (!json_output.empty()) {
    results_json["comparing"]["inputs"].push_back(baseline.string());
  }
  std::optional<nlohmann::json const> challenger_json;
  if (baseline == challenger) {
    if (!json_output.empty()) {
      results_json["comparing"]["mode"] = "cxxet vs. bare (overhead)";
    }

    // suboptimal to iterate over it twice with mutually complementary
    // criterions:
    extract_measurements<kind_e::k_baseline>(results, baseline_json, "bare");
    extract_measurements<kind_e::k_challenger>(results, baseline_json, "cxxet");
  } else {
    if (!json_output.empty()) {
      results_json["comparing"]["mode"] = "two cxxet versions";
      results_json["comparing"]["inputs"].push_back(challenger.string());
    }

    challenger_json.emplace(nlohmann::json::parse(std::ifstream{challenger}));

    extract_measurements<kind_e::k_baseline>(results, baseline_json, "cxxet");
    extract_measurements<kind_e::k_challenger>(results, *challenger_json,
                                               "cxxet");
  }

  long long cnt{0};
  long long cnt_miss_baseline{0};
  long long cnt_miss_challenger{0};
  long long cnt_draw{0};
  long long cnt_challenger_better{0};
  long long cnt_challenger_worse{0};

  for (auto const &[measurement_key, vals] : results) {
    ++cnt;
    auto const measurement_key_str{measurement_key.to_string()};

    if (vals[baseline_ind].empty()) {
      ++cnt_miss_baseline;

      cxxet_pp::log("No baseline data for measurement " + measurement_key_str +
                    "; skipping comparison");

      if (!json_output.empty()) {
        results_json["comparison_results"][base_miss].push_back(
            measurement_key_str);
      }

      continue;
    } else if (vals[challenger_ind].empty()) {
      ++cnt_miss_challenger;

      cxxet_pp::log("No challenger data for measurement " +
                    measurement_key_str + "; skipping comparison");

      if (!json_output.empty()) {
        results_json["comparison_results"][chall_miss].push_back(
            measurement_key_str);
      }

      continue;
    }

    auto const res{
        mann_whitney_u_test(vals[baseline_ind], vals[challenger_ind])};

    static constexpr double p_threshold{0.05};

    bool const probably_different{res.p_value <= p_threshold};

    if (probably_different) {
      auto const stats_baseline{
          cxxet_pp::stats::compute_from(vals[baseline_ind])};
      auto const stats_challenger{
          cxxet_pp::stats::compute_from(vals[challenger_ind])};

      bool const challenger_better{stats_challenger.mean < stats_baseline.mean};

      if (challenger_better) {
        ++cnt_challenger_better;
      } else {
        ++cnt_challenger_worse;
      }

      auto const base_stats_str = [](cxxet_pp::stats const &s) {
        return "n=" + std::to_string(s.cnt) +
               ", mean=" + std::to_string(s.mean) +
               ", stddev=" + std::to_string(s.stddev);
      };

      cxxet_pp::log("Challenger is most probably " +
                    std::string{challenger_better ? "BETTER" : "WORSE"} +
                    " than baseline (" + base_stats_str(stats_challenger) +
                    " vs. " + base_stats_str(stats_baseline) +
                    ") for measurement " + measurement_key_str);

      if (!json_output.empty()) {
        // even though it's used only one time ... `clangd` got confused by the
        // nested brackets, etc. and got broken around those strings being
        // provided directly, at the usage site:
        static constexpr std::string_view measurement{"measurement"};
        static constexpr std::string_view challenger_mean{"challenger_mean"};
        static constexpr std::string_view challenger_stddev{
            "challenger_stddev"};
        static constexpr std::string_view baseline_mean{"baseline_mean"};
        static constexpr std::string_view baseline_stddev{"baseline_stddev"};

        results_json["comparison_results"]
                    [challenger_better ? p_chall_better : p_chall_worse]
                        .push_back({
                            {measurement, measurement_key_str},
                            {challenger_mean, stats_challenger.mean},
                            {challenger_stddev, stats_challenger.stddev},
                            {baseline_mean, stats_baseline.mean},
                            {baseline_stddev, stats_baseline.stddev},
                        });
      }
    } else {
      ++cnt_draw;

      cxxet_pp::log("Challenger is most probably EQUIVALENT to baseline for "
                    "measurement " +
                    measurement_key_str);

      if (!json_output.empty()) {
        results_json["comparison_results"][p_eq].push_back(measurement_key_str);
      }
    }
  }

  auto const compute_pct = [cnt](long long const val) -> double {
    return (static_cast<double>(val) / static_cast<double>(cnt)) * 100.0;
  };

  auto const pct_miss_baseline{compute_pct(cnt_miss_baseline)};
  auto const pct_miss_challenger{compute_pct(cnt_miss_challenger)};
  auto const pct_draw{compute_pct(cnt_draw)};
  auto const pct_challenger_better{compute_pct(cnt_challenger_better)};
  auto const pct_challenger_worse{compute_pct(cnt_challenger_worse)};

  cxxet_pp::log("");
  cxxet_pp::log("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");
  cxxet_pp::log("");
  cxxet_pp::log("Summary of comparisons:");
  cxxet_pp::log("  Total measurements compared: " + std::to_string(cnt));
  cxxet_pp::log(
      "  Missing baseline measurements: " + std::to_string(cnt_miss_baseline) +
      " (" + std::to_string(pct_miss_baseline) + " %)");
  cxxet_pp::log("  Missing challenger measurements: " +
                std::to_string(cnt_miss_challenger) + " (" +
                std::to_string(pct_miss_challenger) + " %)");
  cxxet_pp::log("  Draws: " + std::to_string(cnt_draw) + " (" +
                std::to_string(pct_draw) + " %)");
  cxxet_pp::log(
      "  Challenger better: " + std::to_string(cnt_challenger_better) + " (" +
      std::to_string(pct_challenger_better) + " %)");
  cxxet_pp::log("  Challenger worse: " + std::to_string(cnt_challenger_worse) +
                " (" + std::to_string(pct_challenger_worse) + " %)");

  if (!json_output.empty()) {
    auto &total{results_json["total"]};
    auto const write_total = [&total](std::string_view const key,
                                      long long const val, double const pct) {
      total[key] = {
          {"cnt", val},
          {"pct", pct},
      };
    };

    write_total("n", cnt, 100.0);
    write_total("missing_baseline", cnt_miss_baseline, pct_miss_baseline);
    write_total("missing_challenger", cnt_miss_challenger, pct_miss_challenger);
    write_total("draws", cnt_draw, pct_draw);
    write_total("challenger_better", cnt_challenger_better,
                pct_challenger_better);
    write_total("challenger_worse", cnt_challenger_worse, pct_challenger_worse);

    std::ofstream{json_output} << results_json.dump(json_indent);

    cxxet_pp::log("Saved comparison results into " + json_output.string(),
                  true);
  }
}

} // namespace cxxet_cmp

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
#include <vector>

#include <nlohmann/json.hpp>

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
};

enum class kind_e : unsigned { k_baseline = 0, k_challenger = 1 };

constexpr unsigned get_index(kind_e const k) noexcept {
  return static_cast<unsigned>(k);
}

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
                   std::string_view const /*output*/,
                   int const /*json_indent*/) {
  std::map<measurement, std::array<std::vector<double>, 2>> results;

  nlohmann::json const baseline_json{
      nlohmann::json::parse(std::ifstream{baseline})};
  std::optional<nlohmann::json const> challenger_json;
  if (baseline == challenger) {
    // suboptimal to iterate over it twice with mutually complementary
    // criterions:
    extract_measurements<kind_e::k_baseline>(results, baseline_json, "bare");
    extract_measurements<kind_e::k_challenger>(results, baseline_json, "cxxet");
  } else {
    challenger_json.emplace(nlohmann::json::parse(std::ifstream{challenger}));

    extract_measurements<kind_e::k_baseline>(results, baseline_json, "cxxet");
    extract_measurements<kind_e::k_challenger>(results, challenger_json,
                                               "cxxet");
  }
}

} // namespace cxxet_cmp

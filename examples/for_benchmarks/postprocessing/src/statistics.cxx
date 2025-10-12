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

#include "statistics.hxx"

#include <cmath>

#include <algorithm>
#include <string>

namespace cxxet_pp {

namespace {

double percentile(std::vector<double> const &sorted_values, double const p) {
  if ((p < 0.0) || (p > 100.0)) {
    throw "invalid percentile " + std::to_string(p);
  }
  if (p == 100.0) {
    return sorted_values.back();
  }
  auto const n{static_cast<double>(sorted_values.size())};
  auto const idx{static_cast<std::size_t>(p / 100.0 * n)};
  return sorted_values[idx];
}

} // namespace

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

  return stats{static_cast<long long>(n),
               mean,
               stddev,
               min,
               percentile(sorted_values, 2.0),
               percentile(sorted_values, 25.0),
               percentile(sorted_values, 50.0),
               percentile(sorted_values, 75.0),
               percentile(sorted_values, 98.0),
               max};
}

} // namespace cxxet_pp

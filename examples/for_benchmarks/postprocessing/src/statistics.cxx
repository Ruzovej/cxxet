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
#include <type_traits>

namespace cxxet_pp {

namespace {

constexpr std::size_t percentile_index(std::size_t const n, double const p) {
  auto const idx{p / 100.0 * static_cast<double>(n - 1)};

  return static_cast<std::size_t>(idx + 0.5);
}

constexpr auto c{stats::cnt_for_all_percentiles};
static_assert(c <= 51);

// ------
static_assert(percentile_index(c, 0.0) == 0);
static_assert(percentile_index(c, 1.0) == 0);
static_assert(percentile_index(c, 2.0) == 1);
static_assert(percentile_index(c, 2.0) != percentile_index(c, 0.0));

static_assert(percentile_index(c, 24.0) == c / 4);
static_assert(percentile_index(c, 25.0) == c / 4);
static_assert(percentile_index(c, 26.0) == c / 4 + 1);

static_assert(percentile_index(c, 49.0) == c / 2 - 1);
static_assert(percentile_index(c, 50.0) == c / 2);
static_assert(percentile_index(c, 51.0) == c / 2);

static_assert(percentile_index(c, 74.0) == 3 * c / 4 - 1);
static_assert(percentile_index(c, 75.0) == 3 * c / 4);
static_assert(percentile_index(c, 76.0) == 3 * c / 4);

static_assert(percentile_index(c, 98.0) == c - 2);
static_assert(percentile_index(c, 99.0) == c - 1);
static_assert(percentile_index(c, 100.0) == c - 1);
static_assert(percentile_index(c, 100.0) != percentile_index(c, 98.0));

// ------
static_assert(percentile_index(51, 0.0) == 0);
static_assert(percentile_index(51, 1.0) == 1);
static_assert(percentile_index(51, 2.0) == 1);

static_assert(percentile_index(51, 24.0) == 12);
static_assert(percentile_index(51, 25.0) == 13);
static_assert(percentile_index(51, 26.0) == 13);

static_assert(percentile_index(51, 49.0) == 25);
static_assert(percentile_index(51, 50.0) == 25);
static_assert(percentile_index(51, 51.0) == 26);

static_assert(percentile_index(51, 74.0) == 37);
static_assert(percentile_index(51, 75.0) == 38);
static_assert(percentile_index(51, 76.0) == 38);

static_assert(percentile_index(51, 98.0) == 49);
static_assert(percentile_index(51, 99.0) == 50);
static_assert(percentile_index(51, 100.0) == 50);

// ------
static_assert(percentile_index(100, 0.0) == 0);
static_assert(percentile_index(100, 1.0) == 1);
static_assert(percentile_index(100, 2.0) == 2);

static_assert(percentile_index(100, 24.0) == 24);
static_assert(percentile_index(100, 25.0) == 25);
static_assert(percentile_index(100, 26.0) == 26);

static_assert(percentile_index(100, 49.0) == 49);
static_assert(percentile_index(100, 50.0) == 50);
static_assert(percentile_index(100, 51.0) == 50);

static_assert(percentile_index(100, 74.0) == 73);
static_assert(percentile_index(100, 75.0) == 74);
static_assert(percentile_index(100, 76.0) == 75);

static_assert(percentile_index(100, 98.0) == 97);
static_assert(percentile_index(100, 99.0) == 98);
static_assert(percentile_index(100, 100.0) == 99);

// ------
static_assert(percentile_index(1000, 0.0) == 0);
static_assert(percentile_index(1000, 1.0) == 10);
static_assert(percentile_index(1000, 2.0) == 20);

static_assert(percentile_index(1000, 24.9) == 249);
static_assert(percentile_index(1000, 25.0) == 250);
static_assert(percentile_index(1000, 25.1) == 251);

static_assert(percentile_index(1000, 49.9) == 499);
static_assert(percentile_index(1000, 50.0) == 500);
static_assert(percentile_index(1000, 50.1) == 500);

static_assert(percentile_index(1000, 74.9) == 748);
static_assert(percentile_index(1000, 75.0) == 749);
static_assert(percentile_index(1000, 75.1) == 750);

static_assert(percentile_index(1000, 98.0) == 979);
static_assert(percentile_index(1000, 99.0) == 989);
static_assert(percentile_index(1000, 100.0) == 999);

// ------
static_assert(percentile_index(10000, 0.0) == 0);
static_assert(percentile_index(10000, 1.0) == 100);
static_assert(percentile_index(10000, 2.0) == 200);

static_assert(percentile_index(10000, 24.99) == 2499);
static_assert(percentile_index(10000, 25.0) == 2500);
static_assert(percentile_index(10000, 25.01) == 2501);

static_assert(percentile_index(10000, 49.99) == 4999);
static_assert(percentile_index(10000, 50.0) == 5000);
static_assert(percentile_index(10000, 50.01) == 5000);

static_assert(percentile_index(10000, 74.99) == 7498);
static_assert(percentile_index(10000, 75.0) == 7499);
static_assert(percentile_index(10000, 75.01) == 7500);

static_assert(percentile_index(10000, 98.0) == 9799);
static_assert(percentile_index(10000, 99.0) == 9899);
static_assert(percentile_index(10000, 100.0) == 9999);

double percentile(std::vector<double> const &sorted_values, double const p) {
  if ((p < 0.0) || (p > 100.0)) {
    throw "invalid percentile " + std::to_string(p);
  }
  return sorted_values[percentile_index(sorted_values.size(), p)];
}

} // namespace

stats stats::compute_from(std::vector<double> &&values) {
  if (values.empty()) {
    throw "cannot compute stats over empty vector";
  }

  std::sort(values.begin(), values.end());

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

  return stats{static_cast<long long>(n),
               mean,
               stddev,
               percentile(values, 0.0),
               percentile(values, 2.0),
               percentile(values, 25.0),
               percentile(values, 50.0),
               percentile(values, 75.0),
               percentile(values, 98.0),
               percentile(values, 100.0)};
}

} // namespace cxxet_pp

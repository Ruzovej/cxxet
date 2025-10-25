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

#include "mann_whitney_u_test.hxx"

#include <cmath>

#include <algorithm>

namespace cxxet_cmp {

namespace {

// Helper function: Standard normal CDF
double normal_cdf(double x) {
  return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
}

} // namespace

MannWhitneyResult mann_whitney_u_test(std::vector<double> const &sample1,
                                      std::vector<double> const &sample2) {
  std::size_t const n1{sample1.size()};
  std::size_t const n2{sample2.size()};

  if (n1 == 0 || n2 == 0) {
    return {0.0, 0.0, 0.0, 0.0, 1.0};
  }

  // Combine samples with their group labels
  struct RankedValue {
    double value;
    int group; // 1 or 2
  };

  std::vector<RankedValue> combined;
  combined.reserve(n1 + n2);

  for (auto const &val : sample1) {
    combined.push_back({val, 1});
  }
  for (auto const &val : sample2) {
    combined.push_back({val, 2});
  }

  // Sort by value
  std::sort(combined.begin(), combined.end(),
            [](const RankedValue &a, const RankedValue &b) {
              return a.value < b.value;
            });

  // Assign ranks (handle ties by averaging ranks)
  std::vector<double> ranks(combined.size());
  std::size_t i{0};

  while (i < combined.size()) {
    std::size_t j{i};
    // Find all equal values
    while (j < combined.size() && combined[j].value == combined[i].value) {
      ++j;
    }

    // Average rank for tied values
    auto const avg_rank{static_cast<double>(i + 1 + j) /
                        2.0}; // Ranks are 1-indexed
    for (std::size_t k{i}; k < j; ++k) {
      ranks[k] = avg_rank;
    }

    i = j;
  }

  // Sum ranks for each group
  double R1{0.0}, R2{0.0};
  for (std::size_t k{0}; k < combined.size(); ++k) {
    if (combined[k].group == 1) {
      R1 += ranks[k];
    } else {
      R2 += ranks[k];
    }
  }

  // Calculate U statistics
  auto const U1{static_cast<double>(n1 * n2) +
                static_cast<double>(n1 * (n1 + 1)) / 2.0 - R1};
  auto const U2{static_cast<double>(n1 * n2) +
                static_cast<double>(n2 * (n2 + 1)) / 2.0 - R2};
  auto const U{std::min(U1, U2)};

  // Calculate z-score for large sample approximation
  auto const mean_U{static_cast<double>(n1 * n2) / 2.0};

  // Correction for ties
  double tie_correction{0.0};
  i = 0;
  while (i < combined.size()) {
    std::size_t j{i};
    while ((j < combined.size()) && (combined[j].value == combined[i].value)) {
      ++j;
    }
    std::size_t const t{j - i};
    if (t > 1) {
      tie_correction += static_cast<double>(t * t * t - t);
    }
    i = j;
  }

  auto const n{static_cast<double>(n1 + n2)};
  auto const std_U{
      std::sqrt((n1 * n2 / 12.0) * ((n + 1) - tie_correction / (n * (n - 1))))};

  auto const z_score{(U - mean_U) / std_U};

  // Two-tailed p-value
  auto const p_value{2.0 * (1.0 - normal_cdf(std::abs(z_score)))};

  return {U1, U2, U, z_score, p_value};
}

} // namespace cxxet_cmp

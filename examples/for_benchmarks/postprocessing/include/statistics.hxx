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

#pragma once

#include <vector>

namespace cxxet_pp {

struct stats {
  long long cnt;
  double mean;
  double stddev;
  double min;
  double p02;
  double p25;
  double p50; // median
  double p75;
  double p98;
  double max;

  constexpr bool percentiles_near_min_max_meaningful() const noexcept {
    return cnt >= 50;
  }
};

// `std::span` would be nicer ...
stats compute_stats(std::vector<double> const &values,
                    bool const sort_values = true);

} // namespace cxxet_pp

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

namespace cxxet_cmp {

// https://claude.ai/share/bfc780ad-7860-45dc-97cc-96bbe894148c

struct MannWhitneyResult {
  double U1;      // U statistic for sample 1
  double U2;      // U statistic for sample 2
  double U;       // min(U1, U2)
  double z_score; // Z-score for normal approximation
  double p_value; // Two-tailed p-value
};

MannWhitneyResult mann_whitney_u_test(std::vector<double> const &sample1,
                                      std::vector<double> const &sample2);

} // namespace cxxet_cmp

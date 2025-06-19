/*
  Copyright 2025 Lukáš Růžička (ruzovej@gmail.com, https://github.com/Ruzovej/cxxet)

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#include <cmath>

#include "cxxet/all.hxx"

namespace {

// https://en.wikipedia.org/wiki/Euler_method#First-order_process
template <typename fn_t>
double euler_method(fn_t &&fn, double x, double y, double const h,
                    int const num_steps) {
  for (int i{0}; i < num_steps; ++i) {
    y += h * fn(x, y);
    x += h;
  }

  return y;
}

} // namespace

int main(int argc, char const **argv) {
  [[maybe_unused]] char const *const filename{argc > 1 ? argv[1]
                                                       : "/dev/stdout"};
  CXXET_sink_global_flush(cxxet::output::format::chrome_trace, filename, true);

  CXXET_mark_complete("Counter example 2");

  int const num_points{10'000};
  // step traits:
  int const steps_for_point{4};
  double const h{1e-2 / steps_for_point};
  // initial conditions:
  double x{0.0};
  double y{0.0};
  // differential equation "y' = 1 + 16 * cos(x)"
  // (trivial: solution "y = x + 16 * sin(x)"):
  auto const fn{[](double const xx, double const yy [[maybe_unused]]) {
    return 1.0 + 16.0 * std::cos(xx);
  }};

  {
    CXXET_mark_complete("CXXET_sink_thread_reserve");
    CXXET_sink_thread_reserve(
        num_points * 2 // ...
        + 3 // those 3 extra `CXXET_mark_complete`s above and below ...
    );
  }

  CXXET_mark_complete("Euler method iterations");
  for (int i{0}; i < num_points; ++i) {
    CXXET_mark_counters("y", y, "x", x);
    y = euler_method(fn, x, y, h, steps_for_point);
    x += h * steps_for_point;
  }

  return 0;
}

/*
  Copyright 2025 Lukáš Růžička (ruzovej@gmail.com, https://github.com/Ruzovej/cxxet)

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License along with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#if defined(_WIN32)
#include <windows.h>
#error "Unimplemented platform - TODO ..."
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#include <time.h>
#else
#error "Unsupported platform"
#endif

namespace cxxet::impl {

using timepoint_t = struct timespec;

[[nodiscard]] inline timepoint_t now() noexcept {
  // https://stackoverflow.com/a/42658433
  // https://www.man7.org/linux/man-pages/man3/clock_gettime.3.html
  constexpr int clock_type{
      // choose exactly one of:
      // CLOCK_MONOTONIC // sometimes higher latency
      // CLOCK_MONOTONIC_COARSE // dosn't work (on my primary PC)
      // CLOCK_BOOTTIME // "equivalent" to `CLOCK_MONOTONIC`
      // CLOCK_THREAD_CPUTIME_ID // very low resolution, seems unusable
      CLOCK_MONOTONIC_RAW // seems best
  };
  timepoint_t t;
  clock_gettime(clock_type, &t);
  return t;
}

[[nodiscard]] inline long long as_int_ns(timepoint_t const t) noexcept {
  return static_cast<long long>(t.tv_sec * 1'000'000'000 + t.tv_nsec);
}

} // namespace cxxet::impl

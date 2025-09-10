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

#include <nlohmann/json.hpp>

namespace cxxet_bench {

bool tracing_enabled() noexcept;

struct driver {
  driver(int const argc, char const **argv);
  ~driver();

  long long thread_reserve(int const capacity = 0) const;
  long long set_thread_name(char const *const th_name) const;
  long long thread_flush() const;
  long long global_flush_target(std::string filename) const;
  long long global_flush() const;

  void submit_counter_marker(char const *const name, double const value) const;

  int num_iters;
  int marker_after_iter;
  int cxxet_reserve_buffer;
  int num_threads;
  std::string bench_result_filename;
};

} // namespace cxxet_bench

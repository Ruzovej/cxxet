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

#include <string>

namespace cxxet_bench {

struct driver {
  explicit driver(int const argc, char const **argv);
  ~driver();

  static bool tracing_enabled() noexcept;

  void thread_reserve(int const capacity = 0) const;
  void set_thread_name(char const *const th_name) const;
  void thread_flush() const;
  void global_flush_target() const;
  void global_flush() const;

  void start_marker_submission_measurement() const;
  void stop_marker_submission_measurement() const;

  void submit_counter_marker(char const *const name, double const value) const;
  void submit_instant_marker(char const *const name) const;

  struct complete_marker_alike {
    ~complete_marker_alike() noexcept;

  private:
    explicit complete_marker_alike(char const *const aName) noexcept;
    complete_marker_alike(complete_marker_alike const &) = delete;
    complete_marker_alike(complete_marker_alike &&) = delete;

    friend struct driver;

    alignas(8) unsigned char buffer[32];
  };

  [[nodiscard]] complete_marker_alike
  submit_complete_marker(char const *const name) const;

  void submit_begin_marker(char const *const name) const;
  void submit_end_marker() const;

  int const num_iters;
  // useful only when benchmarking it around doing some nontrivial work (e.g.
  // incrementing atomic) ...:
  int const marker_after_iter;
  int const cxxet_reserve_buffer;
  int const num_threads;

private:
  driver(driver const &) = delete;
  driver(driver &&) = delete;

  std::string const bench_result_filename_base;
};

} // namespace cxxet_bench

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

#include <atomic>
#include <thread>
#include <vector>

#include "cxxet_bench_driver.hxx"

namespace {

void work(std::atomic<int> &ai, [[maybe_unused]] int const th_index,
          [[maybe_unused]] int const num_ths,
          cxxet_bench::driver const &driver) {
  driver.thread_reserve();

  for (int i{0}; i < driver.num_iters; ++i) {
    for (int j{0}; j < driver.marker_after_iter - 1; ++j) {
      ai.fetch_add(1, std::memory_order::memory_order_relaxed);
    }
    auto const val{ai.fetch_add(1, std::memory_order::memory_order_relaxed)};

    driver.submit_counter_marker("some counter ...", val);
  }

  driver.thread_flush();
}

} // namespace

int main(int const argc, char const **argv) {
  cxxet_bench::driver driver{argc, argv};

  std::vector<std::thread> ths;
  ths.reserve(static_cast<std::size_t>(driver.num_threads));
  std::atomic<int> ai{0};

  auto const worker = [&](int const th_index) {
    work(ai, th_index, driver.num_threads, driver);
  };

  for (int i = 1; i < driver.num_threads; ++i) {
    ths.emplace_back(worker, i);
  }

  worker(0);

  for (auto &th : ths) {
    th.join();
  }

#ifndef NDEBUG
  if (ai.load(std::memory_order::memory_order_acquire) !=
      (driver.num_iters * driver.num_threads)) {
    return EXIT_FAILURE;
  }
#endif

  driver.global_flush_target();
  driver.global_flush();

  return EXIT_SUCCESS;
}

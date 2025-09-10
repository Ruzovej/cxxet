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
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>

#include <nlohmann/json.hpp>

#include "common.hxx"
#include "nlohmann/json_fwd.hpp"

namespace {

struct records {
  long long initialization_time_ns{0};
  int th_index;
  int num_ths;
  long long set_flush_name{0};
  long long do_flush{0};
};

void work(std::atomic<int> &ai, int const th_index, int const num_ths,
          cxxet_bench::driver const &driver, records &rec) {
  rec.initialization_time_ns += driver.thread_reserve();

  for (int i{0}; i < driver.num_iters; ++i) {
    for (int j{0}; j < driver.marker_after_iter - 1; ++j) {
      ai.fetch_add(1, std::memory_order::memory_order_relaxed);
    }
    auto const val{ai.fetch_add(1, std::memory_order::memory_order_relaxed)};

    driver.submit_counter_marker("some counter ...", val);
  }

  rec.th_index = th_index;
  rec.num_ths = num_ths;

  driver.thread_flush();
}

} // namespace

int main(int const argc, char const **argv) {
  cxxet_bench::driver const driver{argc, argv};

  std::vector<std::thread> ths;
  ths.reserve(static_cast<std::size_t>(driver.num_threads));
  std::atomic<int> ai{0};
  std::vector<records> recs(static_cast<std::size_t>(driver.num_threads));

  auto const worker = [&](int const th_index) {
    work(ai, th_index, driver.num_threads, driver,
         recs[static_cast<unsigned>(th_index)]);
  };

  for (int i = 1; i < driver.num_threads; ++i) {
    ths.emplace_back(worker, i);
  }

  worker(0);

  for (auto &th : ths) {
    th.join();
  }

  if (ai.load(std::memory_order::memory_order_acquire) !=
      (driver.num_iters * driver.num_threads)) {
    return EXIT_FAILURE;
  }

  auto const filename{
      "/tmp/temporary_whatever_for_now" +
      std::string{cxxet_bench::tracing_enabled() ? "_traced" : ""} + ".json"};

  recs[0].set_flush_name += driver.global_flush_target(filename);
  recs[0].do_flush += driver.global_flush();

  if (!std::filesystem::exists(driver.bench_result_filename)) {
    std::ofstream ofs{driver.bench_result_filename, std::ios::out};
    ofs << "{\"recs\":[]}\n";
    ofs.close();
  }

  nlohmann::json summary =
      nlohmann::json::parse(std::ifstream{driver.bench_result_filename});

  summary["recs"].push_back(
      {{"num_threads", driver.num_threads},
       {"num_iters", driver.num_iters},
       {"marker_after_iter", driver.marker_after_iter},
       {"cxxet_reserve_buffer", driver.cxxet_reserve_buffer},
       {"initialization_time_ns", nlohmann::json::array()},
       {"set_flush_name_ns", recs[0].set_flush_name},
       {"do_flush_ns", recs[0].do_flush}});

  //  nlohmann::json::parse(R"({"pi":3.141,"happy":true})");
  std::cout << summary.dump(2) << std::endl;

  return 0;
}

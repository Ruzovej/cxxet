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

#include <cassert>

#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

#include "cxxet/basic.hxx"
#ifdef CXXET_ENABLE
#include "cxxet/output/writer.hxx"
#include "cxxet/sink_diversion.hxx"
#endif

namespace {

#ifdef CXXET_ENABLE // better then `[[maybe_unused]]`
std::atomic<int> cnt{};
#endif

void test_block() {
  CXXET_sink_thread_reserve();
  {
    CXXET_mark_complete(__FUNCTION__);

    CXXET_mark_counter("counter", cnt.fetch_add(1, std::memory_order_relaxed));

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    CXXET_mark_counter("counter", cnt.load(std::memory_order_relaxed));

    CXXET_mark_instant("within two sleeps");

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    CXXET_mark_counter("counter", cnt.fetch_sub(1, std::memory_order_relaxed));
  } // without this scope, the explicit flush below would "miss" the
    // `CXXET_mark_complete` above
  CXXET_sink_thread_flush_now();
}

std::mutex log_mtx;
void log(std::string_view const msg) {
  std::lock_guard lck{log_mtx};
  std::cout << std::this_thread::get_id() << ": " << msg << '\n';
}

#ifdef CXXET_ENABLE

struct custom_writer final : cxxet::output::writer {
  custom_writer(int const idx) : index{idx} {}
  ~custom_writer() noexcept override = default;

  void prepare_for_writing() override {
    oss << "index " << index << ": Custom writer initialized; output:\n";
  }

  void write(std::string_view const data) override { oss << data; }

  void write(long long const ll) override { oss << ll; }

  void write(unsigned long long const ull) override { oss << ull; }

  void write(double const dp) override { oss << dp; }

  void finalize_and_flush() override {
    oss << "\ncustom writer finished ... index " << index;
    log(oss.str());
  }

private:
  int index;
  std::ostringstream oss;
};

#endif

} // namespace

int main([[maybe_unused]] int argc, [[maybe_unused]] char const **argv) {
#ifdef CXXET_ENABLE
  auto file_sink_local{cxxet::file_sink_handle::make(false)};
  file_sink_local->set_flush_target(std::make_unique<custom_writer>(0));
  file_sink_local->divert_thread_sink_to_this();
#endif

  static constexpr int num_ths{3};
  std::vector<std::thread> ths;
  ths.reserve(num_ths);

  for (int i{1}; i <= num_ths; ++i) {
    log("spawning thread no. " + std::to_string(i));
    ths.emplace_back([i]() {
      log("inside thread no. " + std::to_string(i));
#ifdef CXXET_ENABLE
      auto file_sink_local_th{cxxet::file_sink_handle::make(false)};
      file_sink_local_th->set_flush_target(std::make_unique<custom_writer>(i));
      file_sink_local_th->divert_thread_sink_to_this();
#endif
      test_block();
      log("thread no. " + std::to_string(i) + " finished");
    });
    log("thread no. " + std::to_string(i) + " spawned");
  }

  log("working in main thread");
  test_block();
  log("work in main thread done; joining threads");

  for (auto &th : ths) {
    th.join();
  }

  log("main thread - joined all threads");

  return 0;
}

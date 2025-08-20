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

#include <iostream>
#include <memory>
#include <thread>

#include "cxxet/all.hxx"
#ifdef CXXET_ENABLE
#include "cxxet/output/writer.hxx"
#endif

namespace {

void record_some_events() {
  CXXET_sink_thread_reserve();

  CXXET_mark_complete("complete");

  CXXET_mark_instant("instant");

  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  CXXET_mark_counters("counter", 42.0);

  CXXET_sink_thread_flush_now();
}

#ifdef CXXET_ENABLE

struct custom_writer final : cxxet::output::writer {
  custom_writer() { std::cout << "Custom writer initialized; output:\n"; }
  ~custom_writer() noexcept override = default;

  void write(std::string_view data) override { std::cout << data; }

  void write(long long const ll) override { std::cout << ll; }

  void write(unsigned long long const ull) override { std::cout << ull; }

  void write(double const dp) override { std::cout << dp; }

  void finalize_and_flush() override {
    std::cout << "\ncustom writer finished ..." << std::endl;
  }
};

#endif

} // namespace

int main(int const, char const **) {
  std::thread t{record_some_events};

  record_some_events();

  CXXET_sink_global_set_flush_target(cxxet::output::format::chrome_trace,
                                     std::make_unique<custom_writer>());

  t.join();

  return 0;
}

#include <cstdlib>

#include <iostream>
#include <thread>
#include <vector>

#include "rsm.hpp"

int main(int argc, char const **argv) {
  rsm::init_thread();

  std::vector<int> v(1'000'000, 0);
  rsm::marker m1{"loop"};
  for (int &x : v) {
    x += 1;
  }
  m1.submit();

  [[maybe_unused]] int volatile x;
  std::this_thread::yield();
  rsm::marker m2{"int store"};
  x = 42;
  m2.submit();

  [[maybe_unused]] int y;
  std::this_thread::yield();
  rsm::marker m3{"int load"};
  y = x;
  m3.submit();

  std::thread{[]() {
    rsm::init_thread();
    {
      rsm::marker m{"scoped 1"};
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    rsm::flush_thread();
  }}.join();
  std::thread{[]() {
    rsm::init_thread();
    {
      rsm::marker m{"scoped 2"};
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    // `rsm::flush_thread();` can be omitted
  }}.join();
  std::thread{[]() {
    rsm::init_thread();

    rsm::marker m{"scoped 3"};
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    // no extra scope needed in this case
  }}.join();
  std::thread{[]() {
    rsm::init_thread();

    rsm::marker m{"scoped 4"};
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    m.submit(); // explicit marker submit
  }}.join();
  std::thread{[]() {
    rsm::init_thread();

    RSM_MARKER("scoped 5 (macro with both default color and tag)");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }}.join();
  std::thread{[]() {
    rsm::init_thread();

    RSM_MARKER("scoped 6 (macro with explicit color and default tag)", 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }}.join();
  std::thread{[]() {
    rsm::init_thread();

    RSM_MARKER("scoped 7 (macro with both explicit color and tag)", 1, 2);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }}.join();

  {
    RSM_MARKER("first local macro marker");
    {
      RSM_MARKER("second local macro marker testing no shadowing occurs");
      RSM_MARKER("third local macro marker testing no shadowing occurs");
    }
  }

  rsm::flush_thread(); // this must be done in the main thread, otherwise
  // "local" submitted markers won't be flushed

  { // even after flushing "main" (or the "last" one - dumping collected
    // statistics - to be precise) thread, other threads may still safely
    // contribute:
    constexpr int num_ths{3};
    std::vector<std::thread> ths;
    ths.reserve(num_ths);

    for (int i{0}; i < num_ths; ++i) {
      ths.emplace_back([i]() {
        rsm::init_thread();

        RSM_MARKER("scoped 8 (in 3 various parallel threads)", -1, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(num_ths - i));
      });
    }

    for (auto &th : ths) {
      th.join();
    }
  }

  rsm::output::format const fmt{
      argc > 1 ? static_cast<rsm::output::format>(std::stoi(argv[1]))
               : rsm::output::format::raw_naive_v0};
  char const *const filename{argc > 2 ? argv[2] : nullptr};
  rsm::dump_collected_records(fmt, filename);

  return 0;
}

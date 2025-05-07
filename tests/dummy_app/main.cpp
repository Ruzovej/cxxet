#include <iostream>
#include <thread>
#include <vector>

#include "rsm.hpp"

int main(int, char const **) {
  rsm::init_thread(15);

  std::vector<int> v(1'000'000, 0);
  rsm::marker m1{"loop"};
  for (int &x : v) {
    x += 1;
  }
  m1.submit();

  [[maybe_unused]] int volatile i;
  std::this_thread::yield();
  rsm::marker m2{"int store"};
  i = 42;
  m2.submit();

  [[maybe_unused]] int j;
  std::this_thread::yield();
  rsm::marker m3{"int load"};
  j = i;
  m3.submit();

  std::thread{[]() {
    rsm::init_thread(1);
    {
      rsm::marker m{"scoped 1"};
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    rsm::flush_thread();
  }}.join();
  std::thread{[]() {
    rsm::init_thread(1);
    {
      rsm::marker m{"scoped 2"};
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    // `rsm::flush_thread();` can be omitted
  }}.join();
  std::thread{[]() {
    rsm::init_thread(1);

    rsm::marker m{"scoped 3"};
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    // no extra scope needed in this case
  }}.join();
  std::thread{[]() {
    rsm::init_thread(1);

    rsm::marker m{"scoped 4"};
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    m.submit(); // explicit marker submit
  }}.join();

  rsm::flush_thread(); // this must be done in the main thread, otherwise
  // "local" submitted markers won't be flushed

  rsm::print_flushed_records();

  return 0;
}

#include <iostream>
#include <thread>
#include <vector>

#include "rsm.hpp"

int main(int, char const **) {
  rsm::init_thread(15);

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

  std::vector<int> v(1'000'000);
  rsm::marker m{"loop"};
  for (int &x : v) {
    x += 1;
  }
  m.submit();

  [[maybe_unused]] int volatile i;
  rsm::marker m2{"int assignment"};
  i = 42;
  m2.submit();

  rsm::flush_thread(); // this must be done in the main thread, otherwise
                       // "local" submitted markers won't be flushed
  rsm::impl::global::instance()->print_records();

  return 0;
}
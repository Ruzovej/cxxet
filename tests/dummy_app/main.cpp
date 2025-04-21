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
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    rsm::flush_thread();
  }}.join();
  std::thread{[]() {
    rsm::init_thread(1);
    {
      rsm::marker m{"scoped 2"};
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }}.join();
  std::thread{[]() {
    rsm::init_thread(1);

    rsm::marker m{"scoped 3"};
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }}.join();

  {
    rsm::marker m{"loop"};
    std::vector<int> v(10'000'000);
    for (int &x : v) {
      x += 1;
    }
    std::cout << "Loop took: " << m.submit() << " ns\n";
  }

  rsm::flush_thread();
  std::cout << "runtime markers:\n";
  rsm::impl::global::instance()->print_records();

  return 0;
}
#include <iostream>
#include <thread>
#include <vector>

#include "rsm.hpp"

class Noisy {
  const int ii;

public:
  Noisy(int const i = 0) : ii{i} {
    rsm::marker m{"Noisy ctor"};
    std::cout << "Noisy(" << ii << ")\n";
  }
  ~Noisy() {
    rsm::marker m{"Noisy dtor"};
    std::cout << "~Noisy() with ii = " << ii << "\n";
  }
};

int main(int, char const **) {
  rsm::init_thread(15);

  {
    Noisy n1(1);

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
      rsm::flush_thread();
    }}.join();
    std::thread{[]() {
      rsm::init_thread(1);
      {
        rsm::marker m{"scoped 3"};
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
      rsm::flush_thread();
    }}.join();

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
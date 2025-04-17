#include <climits>

#include <new>
#include <thread>

namespace {

void trigger_tsan() {
  int i{0};
  std::thread{[&i]() { ++i; }}.join();
  ++i;
}

void trigger_ubsan() {
  int i{INT_MAX};
  ++i;
}

void trigger_asan() {
  [[maybe_unused]] int a[2], b[2], c[2];
  volatile unsigned index{2};
  b[index] = 1;
}

void trigger_lsan() { [[maybe_unused]] auto *p{new int[10]}; }

} // namespace

int main(int, char **) {
  trigger_tsan();
  trigger_ubsan();
  trigger_asan();
  trigger_lsan();
  return 0;
}

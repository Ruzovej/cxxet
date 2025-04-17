#include <climits>
#include <cstdlib>

#include <new>
#include <string_view>
#include <thread>

namespace {

void trigger_tsan() {
  int i{0};
  std::thread th{[&i]() { ++i; }};
  ++i;
  th.join();
}

void trigger_ubsan() {
  int i{-1};
  i <<= 1;
}

void trigger_asan() {
  [[maybe_unused]] int a[2], b[2], c[2];
  volatile unsigned index{2};
  b[index] = 1;
}

void trigger_lsan() {
  [[maybe_unused]] auto *volatile p{new int{}};
  [[maybe_unused]] auto *volatile up{std::make_unique<int>().release()};
}

} // namespace

int main(int argc, char **argv) {
  if (argc != 2) {
    return EXIT_FAILURE;
  }

  const std::string_view arg{argv[1]};
  if (arg == "tsan") {
    trigger_tsan();
  } else if (arg == "ubsan") {
    trigger_ubsan();
  } else if (arg == "asan") {
    trigger_asan();
  } else if (arg == "lsan") {
    trigger_lsan();
  } else {
    return EXIT_FAILURE;
  }

  return 0;
}

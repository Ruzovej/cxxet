#include <thread>

#include "cxxst/all.hxx"

static void thread_local_sink_lifecycle() {
  CXXST_sink_thread_reserve(1);
  // do whatever You need between those ...
  CXXST_sink_thread_flush(); // not necessary, it will be flushed
                             // implicitly too - unless You want to do
                             // more work after that ...

  // more work, that doesn't submit any `cxxst` events
}

int main([[maybe_unused]] int const argc, [[maybe_unused]] char const **argv) {
  std::thread{thread_local_sink_lifecycle}.join();
  std::thread t1{thread_local_sink_lifecycle};
  std::thread t2{thread_local_sink_lifecycle};

  thread_local_sink_lifecycle();

  t1.join();
  std::thread{thread_local_sink_lifecycle}.join();
  t2.join();

  CXXST_sink_global_flush(cxxst::output::format::chrome_trace,
                          argc > 1 ? argv[1] : "/dev/stdout");

  return 0;
}

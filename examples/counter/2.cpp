#include <cmath>

#include "rsm.hpp"
#include "rsm/mark_counter.hpp"

namespace {

// https://en.wikipedia.org/wiki/Euler_method#First-order_process
template <typename fn_t>
double euler_method(fn_t &&fn, double x, double y, double const h,
                    int const num_steps) {
  for (int i{0}; i < num_steps; ++i) {
    y += h * fn(x, y);
    x += h;
  }

  return y;
}

} // namespace

int main(int argc, char const **argv) {
  RSM_init_thread_local_sink();

  {
    RSM_MARKER("Counter example 2");

    int const num_points{10'000};
    // step traits:
    int const steps_for_point{4};
    double const h{1e-2 / steps_for_point};
    // initial conditions:
    double x{0.0};
    double y{0.0};
    // differential equation "y' = 1 + 16 * cos(x)"
    // (trivial: solution "y = x + 16 * sin(x)"):
    auto const fn{[](double const xx, double const yy [[maybe_unused]]) {
      return 1.0 + 16.0 * std::cos(xx);
    }};

    {
      RSM_MARKER("RSM_thread_local_sink_reserve");
      RSM_thread_local_sink_reserve(
          num_points * 2 // ...
          + 2            // those 2 extra `RSM_MARKER`s above and below ...
      );
    }

    RSM_MARKER("Euler method iterations");
    for (int i{0}; i < num_points; ++i) {
      RSM_MARK_COUNTERS("y", y, "x", x);
      y = euler_method(fn, x, y, h, steps_for_point);
      x += h * steps_for_point;
    }
  }

  RSM_flush_thread_local_sink();

  char const *const filename{argc > 1 ? argv[1] : "/dev/stdout"};
  RSM_flush_all_collected_events(rsm::output::format::chrome_trace, filename);

  return 0;
}

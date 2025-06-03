#include "rsm/mark_counter.hpp"

#include "impl/event/kind/complete.hpp"
#include "impl/thread_local_sink_submit_event.hpp"

namespace rsm {

void submit_counter(char const *const name, long long const timestamp_ns,
                    double const value) noexcept {
  impl::thread_local_sink_submit_event(
      impl::event::counter{name, timestamp_ns, value});
}

} // namespace rsm

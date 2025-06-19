#include "cxxet/mark_counter.hxx"

#include "impl/event/kind/complete.hxx"
#include "impl/thread_local_sink_submit_event.hxx"

namespace cxxet {

void submit_counter(char const *const name, long long const timestamp_ns,
                    double const value) noexcept {
  impl::thread_local_sink_submit_event(
      impl::event::counter{name, timestamp_ns, value});
}

} // namespace cxxet

#include "cxxet/mark_duration_end.hxx"

#include "impl/event/kind/duration.hxx"
#include "impl/thread_local_sink_submit_event.hxx"

namespace cxxet {

void submit_duration_end(char const *const desc,
                         long long const timestamp_ns) noexcept {
  impl::thread_local_sink_submit_event(
      impl::event::duration_end{desc, timestamp_ns});
}

} // namespace cxxet

#include "rsm/mark_duration_begin.hpp"

#include "impl/event/kind/duration.hpp"
#include "impl/thread_local_sink_submit_event.hpp"

namespace rsm {

RSM_IMPL_API void submit_duration_begin(char const *const desc,
                                        long long const timestamp_ns) noexcept {
  impl::thread_local_sink_submit_event(
      impl::event::duration_begin{desc, timestamp_ns});
}

} // namespace rsm

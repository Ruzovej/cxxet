#include "cxxst/mark_duration_end.hpp"

#include "impl/event/kind/duration.hpp"
#include "impl/thread_local_sink_submit_event.hpp"

namespace cxxst {

CXXST_IMPL_API void submit_duration_end(char const *const desc,
                                        long long const timestamp_ns) noexcept {
  impl::thread_local_sink_submit_event(
      impl::event::duration_end{desc, timestamp_ns});
}

} // namespace cxxst

#include "rsm/mark_complete.hpp"

#include "impl/event/kind/complete.hpp"
#include "impl/thread_local_sink_submit_event.hpp"

namespace rsm {

void mark_complete::submit(impl::timepoint_t const end) noexcept {
  auto const begin{impl::as_int_ns(start)};
  auto const duration{impl::as_int_ns(end) - begin};
  impl::thread_local_sink_submit_event(
      impl::event::complete{desc, begin, duration});
}

} // namespace rsm

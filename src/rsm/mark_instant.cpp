#include "rsm/mark_instant.hpp"

#include "impl/event/kind/instant.hpp"
#include "impl/thread_local_sink_submit_event.hpp"

namespace rsm {

void submit_instant(char const *const desc, scope_t const scope,
                    long long const timestamp_ns) noexcept {
  impl::thread_local_sink_submit_event(
      impl::event::instant{desc, scope, timestamp_ns});
}

} // namespace rsm

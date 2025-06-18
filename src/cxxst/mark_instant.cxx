#include "cxxst/mark_instant.hxx"

#include "impl/event/kind/instant.hxx"
#include "impl/thread_local_sink_submit_event.hxx"

namespace cxxst {

void submit_instant(char const *const desc, scope_t const scope,
                    long long const timestamp_ns) noexcept {
  impl::thread_local_sink_submit_event(
      impl::event::instant{desc, scope, timestamp_ns});
}

} // namespace cxxst

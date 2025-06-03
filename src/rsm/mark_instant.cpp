#include "rsm/mark_instant.hpp"

#include "impl/event/any.hpp"

namespace rsm {

void submit_instant(char const *const desc, scope_t const scope,
                    long long const timestamp_ns) noexcept {
  RSM_IMPL_append_event(impl::event::instant{desc, scope, timestamp_ns});
}

} // namespace rsm

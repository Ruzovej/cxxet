#include "rsm/mark_counter.hpp"

#include "impl/event/any.hpp"
#include "impl/event/kind/complete.hpp"

namespace rsm {

void submit_counter(long long const timestamp_ns, char const *const name,
                    double const value) noexcept {
  RSM_IMPL_append_event(impl::event::counter{name, timestamp_ns, value});
}

} // namespace rsm

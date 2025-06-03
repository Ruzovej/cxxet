#include "rsm/mark_duration_begin.hpp"

#include "impl/event/any.hpp"

namespace rsm {

RSM_IMPL_API void submit_duration_begin(char const *const desc,
                                        long long const timestamp_ns) noexcept {
  RSM_IMPL_append_event(impl::event::duration_begin{desc, timestamp_ns});
}

} // namespace rsm

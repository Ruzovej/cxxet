#include "rsm/mark_complete.hpp"

#include "impl/event/any.hpp"
#include "impl/event/kind/complete.hpp"

namespace rsm {

void mark_complete::submit(impl::timepoint_t const end) noexcept {
  auto const begin{impl::as_int_ns(start)};
  auto const duration{impl::as_int_ns(end) - begin};
  RSM_IMPL_append_event(impl::event::complete{desc, begin, duration});
}

} // namespace rsm

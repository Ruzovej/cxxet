#pragma once

#include "impl/event/any.hpp"
#include "impl/linkage_macros.hpp"
#include "impl/utils.hpp"

#define RSM_mark_instant(...) rsm::mark_instant(__VA_ARGS__)

namespace rsm {

inline void mark_instant(char const *desc,
                         impl::event::instant::scope_t const scope =
                             impl::event::instant::scope_t::thread) noexcept {
  RSM_IMPL_append_event(
      impl::event::instant{desc, scope, impl::as_int_ns(impl::now())});
}

} // namespace rsm

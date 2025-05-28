#pragma once

#include "impl/event/any.hpp"
#include "impl/linkage_macros.hpp"
#include "impl/utils.hpp"

#define RSM_MARK_INSTANT(...) ::rsm::mark_instant(__VA_ARGS__)

namespace rsm {

inline void mark_instant(char const *aDesc,
                         impl::event::instant::scope_t const scope =
                             impl::event::instant::scope_t::thread) noexcept {
  RSM_IMPL_append_event(
      impl::event::instant{aDesc, scope, impl::as_int_ns(impl::now())});
}

} // namespace rsm

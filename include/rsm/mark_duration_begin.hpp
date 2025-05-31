#pragma once

#include "impl/event/any.hpp"
#include "impl/linkage_macros.hpp"
#include "impl/utils.hpp"

#define RSM_MARK_DURATION_BEGIN(...) ::rsm::mark_duration_begin(__VA_ARGS__)

namespace rsm {

inline void mark_duration_begin(char const *desc) noexcept {
  RSM_IMPL_append_event(
      impl::event::duration_begin{desc, impl::as_int_ns(impl::now())});
}

} // namespace rsm

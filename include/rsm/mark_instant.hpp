#pragma once

#include "impl/linkage_macros.hpp"
#include "impl/utils.hpp"
#include "rsm/scope.hpp"

#define RSM_mark_instant(...) rsm::mark_instant(__VA_ARGS__)

namespace rsm {

RSM_IMPL_API void submit_instant(char const *const desc, scope_t const scope,
                                 long long const timestamp_ns) noexcept;

inline void mark_instant(char const *const desc,
                         scope_t const scope = scope_t::thread) noexcept {
  submit_instant(desc, scope, impl::as_int_ns(impl::now()));
}

} // namespace rsm

#pragma once

#include "cxxst/macros/linkage.h"
#include "cxxst/scope.hxx"
#include "cxxst/timepoint.hxx"

namespace cxxst {

CXXST_IMPL_API void submit_instant(char const *const desc, scope_t const scope,
                                   long long const timestamp_ns) noexcept;

inline void mark_instant(char const *const desc,
                         scope_t const scope = scope_t::thread) noexcept {
  submit_instant(desc, scope, impl::as_int_ns(impl::now()));
}

} // namespace cxxst

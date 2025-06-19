#pragma once

#include "cxxet/macros/linkage.h"
#include "cxxet/scope.hxx"
#include "cxxet/timepoint.hxx"

namespace cxxet {

CXXET_IMPL_API void submit_instant(char const *const desc, scope_t const scope,
                                   long long const timestamp_ns) noexcept;

inline void mark_instant(char const *const desc,
                         scope_t const scope = scope_t::thread) noexcept {
  submit_instant(desc, scope, impl::as_int_ns(impl::now()));
}

} // namespace cxxet

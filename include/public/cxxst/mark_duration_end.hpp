#pragma once

#include "cxxst/macros/linkage.h"
#include "cxxst/timepoint.hpp"

namespace cxxst {

CXXST_IMPL_API void submit_duration_end(char const *const desc,
                                      long long const timestamp_ns) noexcept;

inline void mark_duration_end(char const *desc = nullptr) noexcept {
  submit_duration_end(desc, impl::as_int_ns(impl::now()));
}

} // namespace cxxst

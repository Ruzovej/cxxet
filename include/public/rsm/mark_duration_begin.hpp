#pragma once

#include "rsm/macros/linkage.h"
#include "rsm/timepoint.hpp"

#define RSM_mark_duration_begin(...) rsm::mark_duration_begin(__VA_ARGS__)

namespace rsm {

RSM_IMPL_API void submit_duration_begin(char const *const desc,
                                        long long const timestamp_ns) noexcept;

inline void mark_duration_begin(char const *const desc) noexcept {
  submit_duration_begin(desc, impl::as_int_ns(impl::now()));
}

} // namespace rsm

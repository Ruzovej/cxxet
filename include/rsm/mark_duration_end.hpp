#pragma once

#include "impl/linkage_macros.hpp"
#include "impl/utils.hpp"

// Provide either same string as to the corresponding `RSM_mark_duration_begin`
// call, `nullptr` or nothing at all. This is so ui.perfetto.dev processes it
// correctly.
#define RSM_mark_duration_end(...) rsm::mark_duration_end(__VA_ARGS__)

namespace rsm {

RSM_IMPL_API void submit_duration_end(char const *const desc,
                                      long long const timestamp_ns) noexcept;

inline void mark_duration_end(char const *desc = nullptr) noexcept {
  submit_duration_end(desc, impl::as_int_ns(impl::now()));
}

} // namespace rsm

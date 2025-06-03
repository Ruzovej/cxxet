#pragma once

#include "rsm/macros/linkage.h"
#include "rsm/output_format.hpp"

namespace rsm {
// call at most once per thread, and not after `CXXST_thread_local_sink_reserve`:
CXXST_IMPL_API void init_thread_local_sink() noexcept;

CXXST_IMPL_API void
thread_local_sink_reserve(int const minimum_free_capacity) noexcept;

CXXST_IMPL_API void flush_thread_local_sink() noexcept;

CXXST_IMPL_API void flush_global_sink(
    rsm::output::format const fmt = rsm::output::format::chrome_trace,
    char const *const filename = nullptr, // `== nullptr` => no-op; to be more
                                          // precise: discard everything
    bool const defer_flush = false) noexcept;

} // namespace rsm

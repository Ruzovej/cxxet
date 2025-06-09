#pragma once

#include "cxxst/macros/linkage.h"
#include "cxxst/output_format.hpp"

namespace cxxst {

// call before submitting first marker; non-positive value - use
// env. or default setting
CXXST_IMPL_API void
thread_local_sink_reserve(int const minimum_free_capacity = 0) noexcept;

CXXST_IMPL_API void flush_thread_local_sink() noexcept;

CXXST_IMPL_API void sink_global_flush(
    cxxst::output::format const fmt = cxxst::output::format::chrome_trace,
    char const *const filename = nullptr, // `== nullptr` => no-op; to be more
                                          // precise: discard everything
    bool const defer_flush = false) noexcept;

} // namespace cxxst

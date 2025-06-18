#pragma once

#include "cxxst/macros/linkage.h"
#include "cxxst/output_format.hxx"

namespace cxxst {

// call before submitting first marker; non-positive value - use
// env. or default setting
CXXST_IMPL_API void
sink_thread_reserve(int const minimum_free_capacity = 0) noexcept;

CXXST_IMPL_API void sink_thread_flush() noexcept;

CXXST_IMPL_API void sink_global_flush(
    cxxst::output::format const fmt = cxxst::output::format::chrome_trace,
    char const *const filename = nullptr, // `== nullptr` => no-op; to be more
                                          // precise: discard everything
    bool const defer_flush = false) noexcept;

} // namespace cxxst

#pragma once

#include "cxxet/macros/linkage.h"
#include "cxxet/output_format.hxx"

namespace cxxet {

// call before submitting first marker; non-positive value - use
// env. or default setting
CXXET_IMPL_API void
sink_thread_reserve(int const minimum_free_capacity = 0) noexcept;

CXXET_IMPL_API void sink_thread_flush() noexcept;

CXXET_IMPL_API void sink_global_flush(
    cxxet::output::format const fmt = cxxet::output::format::chrome_trace,
    char const *const filename = nullptr, // `== nullptr` => no-op; to be more
                                          // precise: discard everything
    bool const defer_flush = false) noexcept;

} // namespace cxxet

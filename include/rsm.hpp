#pragma once

#include "impl/event/any.hpp"
#include "impl/linkage_macros.hpp"
#include "impl/utils.hpp"
#include "rsm_output_format.hpp"

// call at most once per thread, and not after `RSM_thread_local_sink_reserve`:
RSM_IMPL_API void RSM_init_thread_local_sink() noexcept;

RSM_IMPL_API void
RSM_thread_local_sink_reserve(int const minimum_free_capacity) noexcept;

RSM_IMPL_API void RSM_flush_thread_local_sink() noexcept;

RSM_IMPL_API void RSM_flush_all_collected_events(
    rsm::output::format const fmt = rsm::output::format::chrome_trace,
    char const *const filename = nullptr, // `== nullptr` => no-op; to be more
                                          // precise: discard everything
    bool const defer_flush = false) noexcept;

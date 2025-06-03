#pragma once

#include "rsm/sink_control.hpp"

// call at most once per thread, and not after `RSM_thread_local_sink_reserve`:
inline void RSM_init_thread_local_sink() noexcept {
  rsm::init_thread_local_sink();
}

inline void
RSM_thread_local_sink_reserve(int const minimum_free_capacity) noexcept {
  rsm::thread_local_sink_reserve(minimum_free_capacity);
}

inline void RSM_flush_thread_local_sink() noexcept {
  rsm::flush_thread_local_sink();
}

inline void RSM_flush_global_sink(
    rsm::output::format const fmt = rsm::output::format::chrome_trace,
    char const *const filename = nullptr, // `== nullptr` => no-op; to be more
                                          // precise: discard everything
    bool const defer_flush = false) noexcept {
  rsm::flush_global_sink(fmt, filename, defer_flush);
}

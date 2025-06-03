#pragma once

#include "rsm/macros/implicit_name.h"
#include "rsm/mark_complete.hpp"
#include "rsm/mark_counter.hpp"
#include "rsm/mark_duration.hpp"
#include "rsm/mark_duration_begin.hpp"
#include "rsm/mark_duration_end.hpp"
#include "rsm/mark_instant.hpp"
#include "rsm/output_format.hpp"
#include "rsm/scope.hpp"
#include "rsm/sink_control.hpp"
#include "rsm/timepoint.hpp"

#define CXXST_mark_complete(description)                                         \
  rsm::mark_complete CXXST_IMPL_IMPLICIT_MARKER_NAME(CXXST_IMPLICIT_MARKER_,       \
                                                   __LINE__)(description)

#define CXXST_mark_counters(...) rsm::mark_counters_call(__VA_ARGS__)

#define CXXST_mark_duration(description)                                         \
  rsm::mark_duration CXXST_IMPL_IMPLICIT_MARKER_NAME(CXXST_IMPLICIT_MARKER_,       \
                                                   __LINE__) {                 \
    description                                                                \
  }

#define CXXST_mark_duration_begin(description)                                   \
  rsm::mark_duration_begin(description)

// Provide either same string as to the corresponding `CXXST_mark_duration_begin`
// call, `nullptr` or nothing at all. This is so ui.perfetto.dev processes it
// correctly.
#define CXXST_mark_duration_end(description) rsm::mark_duration_end(description)

#define CXXST_mark_instant(...) rsm::mark_instant(__VA_ARGS__)

// call at most once per thread, and not after `CXXST_thread_local_sink_reserve`:
#define CXXST_init_thread_local_sink() rsm::init_thread_local_sink()

#define CXXST_thread_local_sink_reserve(minimum_free_capacity)                   \
  rsm::thread_local_sink_reserve(minimum_free_capacity)

#define CXXST_flush_thread_local_sink() rsm::flush_thread_local_sink()

#define CXXST_flush_global_sink(...) rsm::flush_global_sink(__VA_ARGS__)

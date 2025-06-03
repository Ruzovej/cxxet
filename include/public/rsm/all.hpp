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

#define RSM_mark_complete(description)                                         \
  rsm::mark_complete RSM_IMPL_IMPLICIT_MARKER_NAME(RSM_IMPLICIT_MARKER_,       \
                                                   __LINE__)(description)

#define RSM_mark_counters(...) rsm::mark_counters_call(__VA_ARGS__)

#define RSM_mark_duration(description)                                         \
  rsm::mark_duration RSM_IMPL_IMPLICIT_MARKER_NAME(RSM_IMPLICIT_MARKER_,       \
                                                   __LINE__) {                 \
    description                                                                \
  }

#define RSM_mark_duration_begin(description)                                   \
  rsm::mark_duration_begin(description)

// Provide either same string as to the corresponding `RSM_mark_duration_begin`
// call, `nullptr` or nothing at all. This is so ui.perfetto.dev processes it
// correctly.
#define RSM_mark_duration_end(description) rsm::mark_duration_end(description)

#define RSM_mark_instant(...) rsm::mark_instant(__VA_ARGS__)

// call at most once per thread, and not after `RSM_thread_local_sink_reserve`:
#define RSM_init_thread_local_sink() rsm::init_thread_local_sink()

#define RSM_thread_local_sink_reserve(minimum_free_capacity)                   \
  rsm::thread_local_sink_reserve(minimum_free_capacity)

#define RSM_flush_thread_local_sink() rsm::flush_thread_local_sink()

#define RSM_flush_global_sink(...) rsm::flush_global_sink(__VA_ARGS__)

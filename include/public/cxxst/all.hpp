#pragma once

#include "cxxst/macros/implicit_name.h"
#include "cxxst/mark_complete.hpp"
#include "cxxst/mark_counter.hpp"
#include "cxxst/mark_duration.hpp"
#include "cxxst/mark_duration_begin.hpp"
#include "cxxst/mark_duration_end.hpp"
#include "cxxst/mark_instant.hpp"
#include "cxxst/output_format.hpp"
#include "cxxst/scope.hpp"
#include "cxxst/sink_control.hpp"
#include "cxxst/timepoint.hpp"

#define CXXST_mark_complete(description)                                         \
  cxxst::mark_complete CXXST_IMPL_IMPLICIT_MARKER_NAME(CXXST_IMPLICIT_MARKER_,       \
                                                   __LINE__)(description)

#define CXXST_mark_counters(...) cxxst::mark_counters_call(__VA_ARGS__)

#define CXXST_mark_duration(description)                                         \
  cxxst::mark_duration CXXST_IMPL_IMPLICIT_MARKER_NAME(CXXST_IMPLICIT_MARKER_,       \
                                                   __LINE__) {                 \
    description                                                                \
  }

#define CXXST_mark_duration_begin(description)                                   \
  cxxst::mark_duration_begin(description)

// Provide either same string as to the corresponding `CXXST_mark_duration_begin`
// call, `nullptr` or nothing at all. This is so ui.perfetto.dev processes it
// correctly.
#define CXXST_mark_duration_end(description) cxxst::mark_duration_end(description)

#define CXXST_mark_instant(...) cxxst::mark_instant(__VA_ARGS__)

// call at most once per thread, and not after `CXXST_thread_local_sink_reserve`:
#define CXXST_init_thread_local_sink() cxxst::init_thread_local_sink()

#define CXXST_thread_local_sink_reserve(minimum_free_capacity)                   \
  cxxst::thread_local_sink_reserve(minimum_free_capacity)

#define CXXST_flush_thread_local_sink() cxxst::flush_thread_local_sink()

#define CXXST_flush_global_sink(...) cxxst::flush_global_sink(__VA_ARGS__)

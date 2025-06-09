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

#define CXXST_mark_complete(description)                                       \
  cxxst::mark_complete CXXST_IMPL_IMPLICIT_MARKER_NAME(CXXST_IMPLICIT_MARKER_, \
                                                       __LINE__)(description)

#define CXXST_mark_counters(...) cxxst::mark_counters_call(__VA_ARGS__)

#define CXXST_mark_duration(description)                                       \
  cxxst::mark_duration CXXST_IMPL_IMPLICIT_MARKER_NAME(CXXST_IMPLICIT_MARKER_, \
                                                       __LINE__) {             \
    description                                                                \
  }

#define CXXST_mark_duration_begin(description)                                 \
  cxxst::mark_duration_begin(description)

// Provide either same string as to the corresponding
// `CXXST_mark_duration_begin` call, `nullptr` or nothing at all. This is so
// ui.perfetto.dev processes it correctly.
#define CXXST_mark_duration_end(description)                                   \
  cxxst::mark_duration_end(description)

#define CXXST_mark_instant(...) cxxst::mark_instant(__VA_ARGS__)

#define CXXST_sink_thread_reserve(minimum_free_capacity)                 \
  cxxst::sink_thread_reserve(minimum_free_capacity)

#define CXXST_sink_thread_flush() cxxst::sink_thread_flush()

#define CXXST_sink_global_flush(...) cxxst::sink_global_flush(__VA_ARGS__)

/*
  Copyright 2025 Lukáš Růžička

  This file is part of cxxet.

  cxxet is free software: you can redistribute it and/or modify it under the
  terms of the GNU Lesser General Public License as published by the Free
  Software Foundation, either version 3 of the License, or (at your option) any
  later version.

  cxxet is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
  details.

  You should have received a copy of the GNU Lesser General Public License along
  with cxxet. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#ifdef CXXET_ENABLE

#include "cxxet/macros/implicit_name.h"
#include "cxxet/mark_complete.hxx"
#include "cxxet/mark_counter.hxx"
#include "cxxet/mark_duration.hxx"
#include "cxxet/mark_duration_begin.hxx"
#include "cxxet/mark_duration_end.hxx"
#include "cxxet/mark_instant.hxx"
#include "cxxet/output_format.hxx"
#include "cxxet/scope.hxx"
#include "cxxet/sink_control.hxx"
#include "cxxet/timepoint.hxx"

#define CXXET_mark_complete(description)                                       \
  cxxet::mark_complete CXXET_IMPL_IMPLICIT_MARKER_NAME(CXXET_IMPLICIT_MARKER_, \
                                                       __LINE__)(description)

#define CXXET_mark_counters(...) cxxet::mark_counters_call(__VA_ARGS__)

#define CXXET_mark_duration(description)                                       \
  cxxet::mark_duration CXXET_IMPL_IMPLICIT_MARKER_NAME(CXXET_IMPLICIT_MARKER_, \
                                                       __LINE__) {             \
    description                                                                \
  }

#define CXXET_mark_duration_begin(description)                                 \
  cxxet::mark_duration_begin(description)

// Provide either same string as to the corresponding
// `CXXET_mark_duration_begin` call, `nullptr` or nothing at all. This is so
// ui.perfetto.dev processes it correctly.
#define CXXET_mark_duration_end(description)                                   \
  cxxet::mark_duration_end(description)

#define CXXET_mark_instant(...) cxxet::mark_instant(__VA_ARGS__)

#define CXXET_sink_thread_reserve(minimum_free_capacity)                       \
  cxxet::sink_thread_reserve(minimum_free_capacity)

#define CXXET_sink_thread_flush() cxxet::sink_thread_flush()

#define CXXET_sink_global_flush(...) cxxet::sink_global_flush(__VA_ARGS__)

#define CXXET_sink_thread_divert_to_sink_global()                              \
  cxxet::sink_thread_divert_to_sink_global()

// TODO either rename this file (e.g. to `all_markers`) or provide (macro?!)
// interface to `sink_diversion.hxx`

#else

#define CXXET_mark_complete(...)

#define CXXET_mark_counters(...)

#define CXXET_mark_duration(...)

#define CXXET_mark_duration_begin(...)

#define CXXET_mark_duration_end(...)

#define CXXET_mark_instant(...)

#define CXXET_sink_thread_reserve(...)

#define CXXET_sink_thread_flush()

#define CXXET_sink_global_flush(...)

#define CXXET_sink_thread_divert_to_sink_global()

#endif

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
#include "cxxet/mark/complete.hxx"
#include "cxxet/mark/counter.hxx"
#include "cxxet/mark/duration.hxx"
#include "cxxet/mark/duration_begin.hxx"
#include "cxxet/mark/duration_end.hxx"
#include "cxxet/mark/instant.hxx"
#include "cxxet/mark/metadata.hxx"
#include "cxxet/scope.hxx"
#include "cxxet/sink_control.hxx"
#include "cxxet/timepoint.hxx"

// -=-=-=-=-=-=-=-=-=-
// core markers/events
// -=-=-=-=-=-=-=-=-=-

#define CXXET_mark_complete(...)                                               \
  cxxet::mark::complete CXXET_IMPL_IMPLICIT_MARKER_NAME(                       \
      CXXET_IMPLICIT_MARKER_, __LINE__) {                                      \
    __VA_ARGS__                                                                \
  }

#define CXXET_mark_counter(...) cxxet::mark::do_submit_counter(__VA_ARGS__)

#define CXXET_mark_counters(...) cxxet::mark::do_submit_counters(__VA_ARGS__)

#define CXXET_mark_duration(...)                                               \
  cxxet::mark::duration CXXET_IMPL_IMPLICIT_MARKER_NAME(                       \
      CXXET_IMPLICIT_MARKER_, __LINE__) {                                      \
    __VA_ARGS__                                                                \
  }

#define CXXET_mark_duration_begin(...) cxxet::mark::duration_begin(__VA_ARGS__)

// Provide either same string as to the corresponding
// `CXXET_mark_duration_begin` call, `nullptr` or nothing at all. This is so
// ui.perfetto.dev processes it correctly.
#define CXXET_mark_duration_end(...) cxxet::mark::duration_end(__VA_ARGS__)

#define CXXET_mark_instant(...) cxxet::mark::instant(__VA_ARGS__)

// -=-=-=-=-=-=-=-=-=-
// meta markers/events
// -=-=-=-=-=-=-=-=-=-

#define CXXET_mark_process_name(...) cxxet::mark::process_name(__VA_ARGS__)

#define CXXET_mark_process_label(...) cxxet::mark::process_label(__VA_ARGS__)

#define CXXET_mark_process_sort_index(...)                                     \
  cxxet::mark::process_sort_index(__VA_ARGS__)

#define CXXET_mark_thread_name(...) cxxet::mark::thread_name(__VA_ARGS__)

#define CXXET_mark_thread_sort_index(...)                                      \
  cxxet::mark::thread_sort_index(__VA_ARGS__)

// -=-=-=-=-=-=-=-=-
// sink manipulation
// -=-=-=-=-=-=-=-=-

#define CXXET_sink_thread_reserve(minimum_free_capacity)                       \
  cxxet::sink_thread_reserve(minimum_free_capacity)

#define CXXET_sink_thread_flush_now() cxxet::sink_thread_flush_now()

#define CXXET_sink_global_set_flush_target(...)                                \
  cxxet::sink_global_set_flush_target(__VA_ARGS__)

#define CXXET_sink_thread_divert_to_sink_global()                              \
  cxxet::sink_thread_divert_to_sink_global()

#else

#define CXXET_mark_complete(...)

#define CXXET_mark_counter(...)

#define CXXET_mark_counters(...)

#define CXXET_mark_duration(...)

#define CXXET_mark_duration_begin(...)

#define CXXET_mark_duration_end(...)

#define CXXET_mark_instant(...)

#define CXXET_sink_thread_reserve(...)

#define CXXET_mark_process_name(...)

#define CXXET_mark_process_label(...)

#define CXXET_mark_process_sort_index(...)

#define CXXET_mark_thread_name(...)

#define CXXET_mark_thread_sort_index(...)

#define CXXET_sink_thread_flush_now()

#define CXXET_sink_global_set_flush_target(...)

#define CXXET_sink_thread_divert_to_sink_global()

#endif

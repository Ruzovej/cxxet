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

#include "impl/write_out/in_trace_event_format.hxx"

#include <cassert>
#include <cstring>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "impl/event/any.hxx"

namespace cxxet::impl::write_out {

namespace {

std::string escape_json_string(const char *str) {
  if (!str)
    return "null";

  std::ostringstream result;
  result << "\"";

  for (const char *c = str; *c; ++c) {
    switch (*c) {
    case '\"':
      result << "\\\"";
      break;
    case '\\':
      result << "\\\\";
      break;
    case '\b':
      result << "\\b";
      break;
    case '\f':
      result << "\\f";
      break;
    case '\n':
      result << "\\n";
      break;
    case '\r':
      result << "\\r";
      break;
    case '\t':
      result << "\\t";
      break;
    default:
      // Handle control characters
      if (*c >= 0 && *c < 32) {
        result << "\\u" << std::hex << std::setw(4) << std::setfill('0')
               << static_cast<int>(*c);
      } else {
        result << *c;
      }
    }
  }

  result << "\"";
  return result.str();
}

double longlong_ns_to_double_us(long long const ns) noexcept {
  return static_cast<double>(ns) / 1'000.0;
}

} // namespace

void in_trace_event_format(output::writer &out,
                           long long const time_point_zero_ns,
                           event::list const &list) {
  out.prepare_for_writing();

  out << "{\"displayTimeUnit\":\"ns\",";
  // TODO (#86, or create sep. issue for that) put into some "comment" value of
  // `time_point_zero_ns`
  out << "\"traceEvents\":[";

  bool first_record{true};
  list.apply([time_point_zero_ns, &first_record,
              &out](long long const pid, long long const thread_id,
                    event::any const &evt) {
    if (!first_record) {
      out << ',';
    } else {
      first_record = false;
    }

    out << '{';
    out << "\"name\":" << escape_json_string(evt.get_name()) << ',';
    out << "\"ph\":\"" << evt.get_ph() << "\",";
    // TODO (#86, or create sep. issue for that) - start using this:
    // out << "\"cat\":" << escape_json_string(???) << ",";

    switch (evt.get_type()) {
    case event::type_t::duration_begin: {
      auto const &e{evt.evt.dur_begin};

      auto const timestamp{
          longlong_ns_to_double_us(e.start_ns - time_point_zero_ns)};
      out << "\"ts\":" << timestamp << ',';
      break;
    }
    case event::type_t::duration_end: {
      auto const &e{evt.evt.dur_end};

      auto const timestamp{
          longlong_ns_to_double_us(e.end_ns - time_point_zero_ns)};
      out << "\"ts\":" << timestamp << ',';
      break;
    }
    case event::type_t::complete: {
      auto const &e{evt.evt.cmpl};

      auto const start{
          longlong_ns_to_double_us(e.start_ns - time_point_zero_ns)};
      out << "\"ts\":" << start << ',';

      auto const duration{longlong_ns_to_double_us(e.duration_ns)};
      out << "\"dur\":" << duration << ',';
      break;
    }
    case event::type_t::instant: {
      auto const &e{evt.evt.inst};

      auto const timestamp{
          longlong_ns_to_double_us(e.timestamp_ns - time_point_zero_ns)};
      out << "\"ts\":" << timestamp << ',';

      auto const scope{static_cast<std::underlying_type_t<scope_t>>(e.scope)};
      out << "\"s\":\"" << scope << "\",";
      break;
    }
    case event::type_t::counter: {
      auto const &e{evt.evt.cntr};

      auto const timestamp{
          longlong_ns_to_double_us(e.timestamp_ns - time_point_zero_ns)};
      out << "\"ts\":" << timestamp << ',';

      out << "\"args\":{" << escape_json_string(e.get_quantity_name()) << ":"
          << e.value << "},";
      break;
    }
    default: {
      throw std::runtime_error("Unknown event type");
    }
    }

    out << "\"pid\":" << pid << ',';
    out << "\"tid\":" << thread_id;
    out << '}';
  });

  out << ']'; // traceEvents
  out << '}';

  out.finalize_and_flush();
}

} // namespace cxxet::impl::write_out

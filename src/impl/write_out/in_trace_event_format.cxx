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

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "impl/event/any.hxx"

namespace cxxet::impl::write_out {

namespace {

// TODO (https://github.com/Ruzovej/cxxet/issues/137) add cache ...
std::string escape_json_string(char const *const str) {
  if (!str)
    return "null";

  std::ostringstream result;
  result << '"';

  for (const char *c = str; *c; ++c) {
    switch (*c) {
    case '"':
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
      if ((0 <= *c) && (*c < 32)) {
        result << "\\u" << std::hex << std::setw(4) << std::setfill('0')
               << static_cast<int>(*c);
      } else {
        result << *c;
      }
    }
  }

  result << '"';
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

  {
    out << "{\"displayTimeUnit\":\"ns\",";
    // TODO (https://github.com/Ruzovej/cxxet/issues/138) put into some
    // "comment" value of `time_point_zero_ns`
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
      // TODO (https://github.com/Ruzovej/cxxet/issues/139) - start using
      // "category", e.g.: out << "\"cat\":" << escape_json_string(???) << ",";

      auto const write_out_timestamp = [&out](long long const ns) {
        out << "\"ts\":" << longlong_ns_to_double_us(ns) << ',';
      };

      switch (evt.get_type()) {
      case event::type_t::duration_begin: {
        auto const &e{evt.evt.dur_begin};
        write_out_timestamp(e.start_ns - time_point_zero_ns);
        break;
      }
      case event::type_t::duration_end: {
        auto const &e{evt.evt.dur_end};
        write_out_timestamp(e.end_ns - time_point_zero_ns);
        break;
      }
      case event::type_t::complete: {
        auto const &e{evt.evt.cmpl};
        write_out_timestamp(e.start_ns - time_point_zero_ns);
        out << "\"dur\":" << longlong_ns_to_double_us(e.duration_ns) << ',';
        break;
      }
      case event::type_t::instant: {
        auto const &e{evt.evt.inst};
        write_out_timestamp(e.timestamp_ns - time_point_zero_ns);
        out << "\"s\":\""
            << static_cast<std::underlying_type_t<scope_t>>(e.scope) << "\",";
        break;
      }
      case event::type_t::counter: {
        auto const &e{evt.evt.cntr};
        write_out_timestamp(e.timestamp_ns - time_point_zero_ns);
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
  }

  out.finalize_and_flush();
}

} // namespace cxxet::impl::write_out

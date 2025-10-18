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

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "cxxet/get_process_id.hxx"
#include "impl/event/any.hxx"

namespace cxxet::impl::write_out {

namespace {

// TODO (https://github.com/Ruzovej/cxxet/issues/137) add cache ...
std::string escape_json_string(char const *const str, std::size_t len = 0) {
  if (!str) {
    return "null";
  }

  if (len == 0) {
    len = std::strlen(str);
  }

  std::ostringstream result;
  result << '"';

  for (const char *c = str; c != str + len; ++c) {
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

class fraction_us {
  long long us;
  long long frac; // in [0, 999]

  static constexpr int divisor{1'000};

public:
  explicit fraction_us(long long const ns) noexcept
      : us{ns / divisor}, frac{ns % divisor} {
    assert(ns >= 0);
  }

  friend output::writer &operator<<(output::writer &ow, fraction_us const &fu) {
    ow << fu.us << '.';

    // ugh ...
    if (fu.frac < 100) {
      ow << '0';
    }
    if (fu.frac < 10) {
      ow << '0';
    }
    ow << fu.frac;

    return ow;
  }
};

} // namespace

void in_trace_event_format(output::writer &out,
                           long long const time_point_zero_ns,
                           event::list const &list,
                           write_out::category_name_map const &cat_names) {
  out.prepare_for_writing();

  auto const process_id{get_process_id()};

  out << "{\"displayTimeUnit\":\"ns\",";
  // TODO (https://github.com/Ruzovej/cxxet/issues/138) put into some
  // "comment" value of `time_point_zero_ns`
  out << "\"traceEvents\":[";

  long long count{0};
  for (auto const &[thread_id, evt] : list) {
    if (++count > 1) {
      out << ',';
    }

    out << '{';
    out << "\"name\":" << escape_json_string(evt.get_name()) << ',';
    out << "\"ph\":\"" << evt.get_ph() << "\",";
    auto const categories_str{
        cat_names.get_joined_category_names(evt.get_categories())};
    if (!categories_str.empty()) {
      // requirements imply no need to escape it
      out << "\"cat\":"
          << escape_json_string(categories_str.data(), categories_str.size())
          << ",";
    }

    auto const write_out_timestamp = [&out](long long const ns) {
      out << "\"ts\":" << fraction_us{ns} << ',';
    };

    switch (evt.get_type()) {
    case event::trace_type::duration_begin: {
      auto const &e{evt.evt.dur_begin};
      write_out_timestamp(e.start_ns - time_point_zero_ns);
      break;
    }
    case event::trace_type::duration_end: {
      auto const &e{evt.evt.dur_end};
      write_out_timestamp(e.end_ns - time_point_zero_ns);
      break;
    }
    case event::trace_type::complete: {
      auto const &e{evt.evt.cmpl};
      write_out_timestamp(e.start_ns - time_point_zero_ns);
      out << "\"dur\":" << fraction_us{e.duration_ns} << ',';
      break;
    }
    case event::trace_type::instant: {
      auto const &e{evt.evt.inst};
      write_out_timestamp(e.timestamp_ns - time_point_zero_ns);
      out << "\"s\":\"" << static_cast<std::underlying_type_t<scope_t>>(e.scope)
          << "\",";
      break;
    }
    case event::trace_type::counter: {
      auto const &e{evt.evt.cntr};
      write_out_timestamp(e.timestamp_ns - time_point_zero_ns);
      out << "\"args\":{" << escape_json_string(e.get_quantity_name()) << ":"
          << e.value << "},";
      break;
    }
    case event::trace_type::metadata: {
      auto const &e{evt.evt.meta};
      out << "\"args\":{" << escape_json_string(e.get_arg_name()) << ":";
      switch (e.get_metadata_type()) {
      case event::metadata_type::process_name:
      case event::metadata_type::thread_name:
      case event::metadata_type::process_labels:
        out << escape_json_string(e.get_arg_value_str());
        break;
      case event::metadata_type::process_sort_index:
      case event::metadata_type::thread_sort_index:
        out << e.get_arg_value_int();
        break;
      default:
        throw std::runtime_error("Unknown metadata_type");
      }
      out << "},";
      break;
    }
    default: {
      throw std::runtime_error("Unknown event type");
    }
    }

    out << "\"pid\":" << process_id << ',';
    out << "\"tid\":" << thread_id;
    out << '}';
  }

  out << ']'; // traceEvents
  out << '}';

  out.finalize_and_flush();
}

} // namespace cxxet::impl::write_out

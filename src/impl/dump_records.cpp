#include "impl/dump_records.hpp"

#include <cstring>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "rsm_output_format.hpp"

namespace rsm::impl {

namespace {

std::string escape_json_string(const char *str) {
  if (!str)
    return "null";

  std::stringstream result;
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

void write_chrome_trace(std::ostream &out, impl::event::list const &list,
                        long long const time_point_zero) {
  // using pid_and_record = std::pair<long long, record const *>;
  // std::vector<pid_and_record> sorted_records;
  //
  // for (records const *block{first}; block != nullptr;
  //     block = block->next.get()) {
  //  for (record const *r{block->data}; r < block->last; ++r) {
  //    sorted_records.emplace_back(block->thread_id, r);
  //  }
  //}
  // std::sort(sorted_records.begin(), sorted_records.end(),
  //          [](const pid_and_record &a, const pid_and_record &b) {
  //            auto const trans = [](pid_and_record const &r) {
  //              return std::tuple(
  //                  r.first,            // `tid` ascending
  //                  r.second->start_ns, // `start` ascending
  //                  -r.second->end_ns   // `end` with minus => descending
  //              );
  //            };
  //            return trans(a) < trans(b);
  //          });

  out << "{\n";
  out << "  \"traceEvents\": [\n";

  bool first_record{true};
  list.apply([time_point_zero, &first_record, &out](long long const pid,
                                                    long long const thread_id,
                                                    event::any const &evt) {
    if (!first_record) {
      out << ",\n";
    } else {
      first_record = false;
    }

    switch (evt.get_type()) {
    case event::type::duration_begin: {
      // TODO handle this
      break;
    }
    case event::type::duration_end: {
      // TODO handle this
      break;
    }
    case event::type::complete: {
      auto const e = evt.evt.cmp;
      // [us]:
      const auto start{static_cast<double>(e.start_ns - time_point_zero) /
                       1'000.0};
      const auto duration{static_cast<double>(e.duration_ns) / 1'000.0};

      // Chrome trace format:
      // https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU
      out << "    {\n";
      out << "      \"name\": " << escape_json_string(e.evt.desc) << ",\n";
      out << "      \"ph\": \"X\",\n";
      out << "      \"ts\": " << start << ",\n";
      out << "      \"dur\": " << duration << ",\n";
      out << "      \"pid\": " << pid << " ,\n";
      out << "      \"tid\": " << thread_id << ",\n";
      out << "    }";
      break;
    }
    case event::type::instant: {
      // TODO handle this
      break;
    }
    case event::type::counter: {
      // TODO handle this
      break;
    }
    default: {
      throw std::runtime_error("Unknown event type");
    }
    }
  });

  if constexpr (false) {
    out << "\n  ]\n";
  } else { // TODO remove or not?
    out << "\n  ],\n";
    out << "  \"displayTimeUnit\": \"ns\"\n";
  }
  out << "}\n";
}

[[deprecated]] void write_naive_v0(std::ostream &out,
                                   impl::event::list const &list) {
  list.apply([&out](long long const /*pid*/, long long const thread_id,
                    event::any const &evt) {
    switch (evt.get_type()) {
    case event::type::duration_begin: {
      // TODO handle this
      break;
    }
    case event::type::duration_end: {
      // TODO handle this
      break;
    }
    case event::type::complete: {
      auto const evt_complete = evt.evt.cmp;
      out << thread_id << ": '" << evt_complete.evt.desc << "', color "
          << static_cast<int>(evt_complete.evt.explicit_padding[0]) << ", tag "
          << static_cast<int>(evt_complete.evt.explicit_padding[1]) << ": "
          << evt_complete.start_ns << " -> "
          << (evt_complete.start_ns + evt_complete.duration_ns) << " ~ "
          << evt_complete.duration_ns << " [ns]\n";
      break;
    }
    case event::type::instant: {
      // TODO handle this
      break;
    }
    case event::type::counter: {
      // TODO handle this
      break;
    }
    default: {
      throw std::runtime_error("Unknown event type");
    }
    }
  });
}

} // namespace

void dump_records(impl::event::list const &list,
                  long long const time_point_zero, output::format const fmt,
                  char const *const filename) {
  std::ofstream file;
  if (filename) {
    if (std::strcmp(filename, "/dev/stdout") == 0) {
      std::cout.flush();
      file.open(filename, std::ios::app);
    } else {
      file.open(filename, std::ios::out);
    }
  }
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file '" +
                             std::string(filename ? filename : "/dev/stdout") +
                             "'!");
  }

  switch (fmt) {
  case output::format::chrome_trace: {
    write_chrome_trace(file, list, time_point_zero);
    break;
  }
  case output::format::raw_naive_v0: {
    write_naive_v0(file, list);
    return;
  }
  default: {
    throw std::runtime_error("Unknown output format specified");
  }
  }

  file.close();
}

} // namespace rsm::impl

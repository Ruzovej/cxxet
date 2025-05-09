#include "impl/dump_records.hpp"

#include <cstring>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

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

void write_chrome_trace(std::ostream &out, records const *first,
                        long long const time_point_zero) {
  out << "{\n";
  out << "  \"traceEvents\": [\n";

  bool first_record{true};
  for (records const *block{first}; block != nullptr; block = block->next) {
    for (record const *r{block->first}; r < block->last; ++r) {
      if (!first_record) {
        out << ",\n";
      } else {
        first_record = false;
      }

      // [ms]:
      const auto start{static_cast<double>(r->start_ns - time_point_zero) /
                       1000.0};
      const auto duration{static_cast<double>(r->end_ns - r->start_ns) /
                          1000.0};

      // Chrome trace format:
      // https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU
      out << "    {\n";
      out << "      \"name\": " << escape_json_string(r->desc) << ",\n";
      out << "      \"cat\": \"rsm," << r->tag << "\",\n";
      out << "      \"ph\": \"X\",\n"; // Complete event (with duration)
      out << "      \"ts\": " << start << ",\n";
      out << "      \"dur\": " << duration << ",\n";
      out << "      \"pid\": 1,\n"; // TODO write correct PID later ...
      out << "      \"tid\": " << block->thread_id << ",\n";
      out << "      \"args\": {\n";
      out << "        \"color\": " << r->color << "\n";
      out << "      }\n";
      out << "    }";
    }
  }

  if constexpr (true) {
    out << "\n  ]\n";
  } else { // TODO remove or not?
    out << "\n  ],\n";
    out << "  \"displayTimeUnit\": \"ns\"\n";
  }
  out << "}\n";
}

void write_raw_json(std::ostream &out, records const *first,
                    long long const time_point_zero) {
  out << "{\n";
  out << "  \"version\": \"0.1\",\n";
  out << "  \"time_point_zero\": " << time_point_zero << ",\n";
  out << "  \"records\": [\n";

  bool first_record{true};
  for (records const *block{first}; block != nullptr; block = block->next) {
    for (record const *r{block->first}; r < block->last; ++r) {
      if (!first_record) {
        out << ",\n";
      } else {
        first_record = false;
      }

      out << "    {\n";
      out << "      \"desc\": " << escape_json_string(r->desc) << ",\n";
      out << "      \"color\": " << r->color << ",\n";
      out << "      \"tag\": " << r->tag << ",\n";
      out << "      \"start_ns\": " << r->start_ns << ",\n";
      out << "      \"end_ns\": " << r->end_ns << ",\n";
      out << "      \"thread_id\": " << block->thread_id << "\n";
      out << "    }";
    }
  }

  out << "\n  ]\n";
  out << "}\n";
}

// TODO check this later ... maybe it's not needed at all?!
void write_raw_binary(std::ostream &out, records const *first,
                      long long const /*time_point_zero*/) {
  // Write a simple header with version
  const uint32_t version = 0;
  out.write(reinterpret_cast<const char *>(&version), sizeof(version));

  // For each record block
  for (records const *block = first; block != nullptr; block = block->next) {
    // Write thread ID
    out.write(reinterpret_cast<const char *>(&block->thread_id),
              sizeof(block->thread_id));

    // Write number of records in this block
    uint32_t count = static_cast<uint32_t>(block->last - block->first);
    out.write(reinterpret_cast<const char *>(&count), sizeof(count));

    // Write each record
    for (record const *r = block->first; r < block->last; ++r) {
      // For strings, write length followed by chars
      if (r->desc) {
        size_t len = strlen(r->desc);
        out.write(reinterpret_cast<const char *>(&len), sizeof(len));
        out.write(r->desc, static_cast<std::streamsize>(len));
      } else {
        size_t len = 0;
        out.write(reinterpret_cast<const char *>(&len), sizeof(len));
      }

      // Write numeric fields
      out.write(reinterpret_cast<const char *>(&r->color), sizeof(r->color));
      out.write(reinterpret_cast<const char *>(&r->tag), sizeof(r->tag));
      out.write(reinterpret_cast<const char *>(&r->start_ns),
                sizeof(r->start_ns));
      out.write(reinterpret_cast<const char *>(&r->end_ns), sizeof(r->end_ns));
    }
  }
}

} // namespace

void dump_records(records const *first, long long const time_point_zero,
                  output::format const fmt, char const *const filename) {
  if (!first) {
    throw std::invalid_argument("Null pointer provided to dump_records");
  }

  std::ofstream file;
  if (filename) {
    file.open(filename, std::ios::out);
  } else {
    std::cout.flush();
    file.open("/dev/stdout", std::ios::app);
  }
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file '" +
                             std::string(filename ? filename : "/dev/stdout") +
                             "'!");
  }

  switch (fmt) {
  case output::format::chrome_trace: {
    write_chrome_trace(file, first, time_point_zero);
    break;
  }
  case output::format::raw_json_v0: {
    write_raw_json(file, first, time_point_zero);
    break;
  }
  case output::format::raw_binary_v0: {
    write_raw_binary(file, first, time_point_zero);
    break;
  }
  default: {
    throw std::runtime_error("Unknown output format specified");
  }
  }

  file.close();
}

} // namespace rsm::impl

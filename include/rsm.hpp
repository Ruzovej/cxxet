#pragma once

#include "impl/utils.hpp"

#define RSM_MARKER_NAME_JOIN(name, line) name##line
#define RSM_MARKER_NAME(name, line) RSM_MARKER_NAME_JOIN(name, line)
#define RSM_MARKER(...)                                                        \
  ::rsm::marker RSM_MARKER_NAME(RSM_EXPLICIT_MARKER_, __LINE__) { __VA_ARGS__ }

namespace rsm {

void init_thread();

void flush_thread() noexcept;

void print_flushed_records();

struct marker {
  inline marker(char const *aDesc, int const aColor = -1,
                int const aTag = -1) noexcept
      : desc{aDesc}, color{aColor}, tag{aTag}, start{impl::now()} {}
  inline ~marker() noexcept { submit(); }

  [[maybe_unused]] inline long long submit() noexcept;

private:
  char const *desc;
  int const color;
  int const tag;
  struct timespec const start;

  void append_record(long long const start_ns, long long const end_ns) noexcept;

  marker(marker const &) = delete;
  marker &operator=(marker const &) = delete;
  marker(marker &&) = delete;
  marker &operator=(marker &&) = delete;
};

[[maybe_unused]] inline long long marker::submit() noexcept {
  if (desc) // [[likely]] // TODO
  {
    auto const now_ns{impl::as_int_ns(impl::now())};
    auto const start_ns{impl::as_int_ns(start)};
    append_record(start_ns, now_ns);
    desc = nullptr;
    return now_ns - start_ns;
  }
  return -1;
}

} // namespace rsm

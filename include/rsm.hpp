#pragma once

#include "impl/utils.hpp"

namespace rsm {

void init_thread(unsigned const block_size);

void flush_thread() noexcept;

void print_flushed_records();

struct marker {
  inline marker(char const *aDesc, int const aColor = -1, int const aTag = -1)
      : desc{aDesc}, color{aColor}, tag{aTag}, start{impl::now()} {}
  inline ~marker() noexcept { submit(); }

  [[maybe_unused]] inline long long submit() noexcept;

private:
  char const *desc;
  int const color;
  int const tag;
  struct timespec const start;

  static void append_record(char const *desc, int const color, int const tag,
                            long long const start_ns,
                            long long const end_ns) noexcept;
};

[[maybe_unused]] inline long long marker::submit() noexcept {
  if (desc) // [[likely]] // TODO
  {
    auto const now_ns{impl::as_int_ns(impl::now())};
    auto const start_ns{impl::as_int_ns(start)};
    append_record(desc, color, tag, start_ns, now_ns);
    desc = nullptr;
    return now_ns - start_ns;
  }
  return -1;
}

} // namespace rsm

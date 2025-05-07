#pragma once

namespace rsm::impl {

struct record {
  char const *desc;
  int color;
  int tag;
  long long start_ns, end_ns;
};

static_assert(
    64 % sizeof(record) == 0,
    "`record` size must be a multiple of 64 [byte] (cache line size?!)");

} // namespace rsm::impl

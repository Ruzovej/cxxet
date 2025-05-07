#pragma once

namespace rsm::impl {

struct record {
  char const *desc;
  long long start_ns, end_ns;
};

} // namespace rsm::impl

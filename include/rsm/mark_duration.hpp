#pragma once

#include "rsm/mark_duration_begin.hpp"
#include "rsm/mark_duration_end.hpp"

#define RSM_mark_duration(...)                                                 \
  rsm::mark_duration RSM_IMPL_IMPLICIT_MARKER_NAME(RSM_IMPLICIT_MARKER_,       \
                                                   __LINE__) {                 \
    __VA_ARGS__                                                                \
  }

namespace rsm {

struct RSM_IMPL_API mark_duration {
  inline mark_duration(char const *desc) noexcept {
    RSM_mark_duration_begin(desc);
  }

  inline ~mark_duration() noexcept { RSM_mark_duration_end(); }

private:
  mark_duration(mark_duration const &) = delete;
  mark_duration &operator=(mark_duration const &) = delete;
  mark_duration(mark_duration &&) = delete;
  mark_duration &operator=(mark_duration &&) = delete;
};

} // namespace rsm

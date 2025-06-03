#pragma once

#include "impl/linkage_macros.hpp"
#include "impl/utils.hpp"

#define RSM_mark_complete(...)                                                 \
  rsm::mark_complete RSM_IMPL_IMPLICIT_MARKER_NAME(RSM_IMPLICIT_MARKER_,       \
                                                   __LINE__)(__VA_ARGS__)

namespace rsm {

struct RSM_IMPL_API mark_complete {
  inline mark_complete(char const *aDesc) noexcept
      : desc{aDesc}, start{impl::now()} {}

  inline ~mark_complete() noexcept { submit(impl::now()); }

private:
  mark_complete(mark_complete const &) = delete;
  mark_complete &operator=(mark_complete const &) = delete;
  mark_complete(mark_complete &&) = delete;
  mark_complete &operator=(mark_complete &&) = delete;

  void submit(impl::timepoint_t const finish) noexcept;

  const char *const desc;
  impl::timepoint_t const start;
};

} // namespace rsm

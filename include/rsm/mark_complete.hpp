#pragma once

#include "impl/event/any.hpp"
#include "impl/event/kind/complete.hpp"
#include "impl/linkage_macros.hpp"
#include "impl/utils.hpp"

#define RSM_mark_complete(...)                                                 \
  rsm::mark_complete RSM_IMPL_IMPLICIT_MARKER_NAME(RSM_IMPLICIT_MARKER_,       \
                                                   __LINE__)(__VA_ARGS__)

namespace rsm {

struct mark_complete {
  inline mark_complete(char const *aDesc) noexcept
      : desc{aDesc}, start{impl::now()} {}

  inline ~mark_complete() noexcept {
    auto const end{impl::as_int_ns(impl::now())};
    auto const begin{impl::as_int_ns(start)};
    auto const duration{end - begin};
    RSM_IMPL_append_event(impl::event::complete{desc, begin, duration});
  }

private:
  mark_complete(mark_complete const &) = delete;
  mark_complete &operator=(mark_complete const &) = delete;
  mark_complete(mark_complete &&) = delete;
  mark_complete &operator=(mark_complete &&) = delete;

  const char *const desc;
  impl::timepoint_t const start;
};

} // namespace rsm

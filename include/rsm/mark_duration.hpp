#pragma once

#include "impl/event/any.hpp"
#include "impl/linkage_macros.hpp"
#include "impl/utils.hpp"

#define RSM_MARK_DURATION(...)                                                 \
  ::rsm::mark_duration RSM_IMPL_IMPLICIT_MARKER_NAME(RSM_IMPLICIT_MARKER_,     \
                                                     __LINE__) {               \
    __VA_ARGS__                                                                \
  }

namespace rsm {

struct RSM_IMPL_API mark_duration {
  inline mark_duration(char const *desc) noexcept {
    RSM_IMPL_append_event(
        impl::event::duration_begin{desc, impl::as_int_ns(impl::now())});
  }

  inline ~mark_duration() noexcept {
    RSM_IMPL_append_event(
        impl::event::duration_end{nullptr, impl::as_int_ns(impl::now())});
  }

private:
  mark_duration(mark_duration const &) = delete;
  mark_duration &operator=(mark_duration const &) = delete;
  mark_duration(mark_duration &&) = delete;
  mark_duration &operator=(mark_duration &&) = delete;
};

} // namespace rsm

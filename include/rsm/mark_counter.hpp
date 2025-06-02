#pragma once

#include "impl/event/any.hpp"
#include "impl/linkage_macros.hpp"
#include "impl/utils.hpp"

#define RSM_mark_counters(...) rsm::mark_counters_call(__VA_ARGS__)

namespace rsm {

template <typename... Args>
void mark_counters(long long const timestamp_ns, char const *const name,
                   double const value, Args &&...args) noexcept {
  RSM_IMPL_append_event(impl::event::counter{name, timestamp_ns, value});
  if constexpr (sizeof...(args) > 0) {
    mark_counters(timestamp_ns, std::forward<Args>(args)...);
  }
}

template <typename... Args>
inline void mark_counters_call(char const *const name, double const value,
                               Args &&...args) noexcept {
  auto const now_ns{impl::as_int_ns(impl::now())};
  mark_counters(now_ns, name, value, std::forward<Args>(args)...);
}

} // namespace rsm

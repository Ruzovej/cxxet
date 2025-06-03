#pragma once

#include "impl/linkage_macros.hpp"
#include "impl/utils.hpp"

#define RSM_mark_counters(...) rsm::mark_counters_call(__VA_ARGS__)

namespace rsm {

RSM_IMPL_API void submit_counter(long long const timestamp_ns,
                                 char const *const name,
                                 double const value) noexcept;

template <typename... Args>
void mark_counters(long long const timestamp_ns, char const *const name,
                   double const value, Args &&...args) noexcept {
  submit_counter(timestamp_ns, name, value);
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

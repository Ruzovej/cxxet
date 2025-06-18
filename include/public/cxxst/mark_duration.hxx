#pragma once

#include "cxxst/mark_duration_begin.hxx"
#include "cxxst/mark_duration_end.hxx"

namespace cxxst {

struct mark_duration {
  inline mark_duration(char const *desc) noexcept { mark_duration_begin(desc); }

  inline ~mark_duration() noexcept { mark_duration_end(); }

private:
  mark_duration(mark_duration const &) = delete;
  mark_duration &operator=(mark_duration const &) = delete;
  mark_duration(mark_duration &&) = delete;
  mark_duration &operator=(mark_duration &&) = delete;
};

} // namespace cxxst

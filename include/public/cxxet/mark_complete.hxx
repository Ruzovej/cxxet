#pragma once

#include "cxxet/macros/linkage.h"
#include "cxxet/timepoint.hxx"

namespace cxxet {

struct CXXET_IMPL_API mark_complete {
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

} // namespace cxxet

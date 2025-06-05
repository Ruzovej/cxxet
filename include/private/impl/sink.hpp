#pragma once

#include "cxxst/output_format.hpp"
#include "impl/event/list/list.hpp"

namespace cxxst::impl {

struct sink {
  explicit sink() noexcept = default;
  virtual ~sink() noexcept = default;

  virtual void flush_to_file(long long const time_point_zero,
                             cxxst::output::format const fmt,
                             char const *const filename) noexcept;

  virtual void drain(sink &other) noexcept;

private:
  sink(sink const &) = delete;
  sink &operator=(sink const &) = delete;
  sink(sink &&) = delete;
  sink &operator=(sink &&) = delete;

protected:
  event::list events;
};

} // namespace cxxst::impl

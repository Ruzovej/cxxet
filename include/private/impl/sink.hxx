#pragma once

#include "cxxet/output_format.hxx"
#include "impl/event/list/list.hxx"

namespace cxxet::impl {

struct sink {
  sink() noexcept;
  virtual ~sink() noexcept;

  void flush_to_file(long long const time_point_zero,
                     cxxet::output::format const fmt,
                     char const *const filename) noexcept;

  virtual void drain(sink &other) noexcept;

protected:
  event::list events;

private:
  sink(sink const &) = delete;
  sink &operator=(sink const &) = delete;
  sink(sink &&) = delete;
  sink &operator=(sink &&) = delete;
};

} // namespace cxxet::impl

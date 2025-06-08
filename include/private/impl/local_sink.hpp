#pragma once

#include "impl/sink.hpp"

namespace cxxst::impl {

struct local_sink : sink {
  explicit local_sink(sink *aParent) noexcept;
  ~local_sink() noexcept override;

  void append_event(event::any const &evt) noexcept;

  void flush() noexcept;

  void reserve(int const minimum_free_capacity) noexcept;

private:
  local_sink(local_sink const &) = delete;
  local_sink &operator=(local_sink const &) = delete;
  local_sink(local_sink &&) = delete;
  local_sink &operator=(local_sink &&) = delete;

  sink *parent;
  int default_node_capacity{};
};

} // namespace cxxst::impl

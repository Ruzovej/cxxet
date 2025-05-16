#pragma once

#include "impl/central_sink.hpp"
#include "impl/record.hpp"
#include "impl/records.hpp"

namespace rsm::impl {

struct local_sink {
  static void init();

  [[nodiscard]] static local_sink *instance() noexcept;

  inline void append_record(record const m) {
    if (active) // [[likely]] // TODO ...
    {
      if (!last || !last->free_capacity()) // [[unlikely]] // TODO ...
      {
        allocate_next_records();
      }
      last->append_record(m);
    }
  }

  void flush_to_central_sink(central_sink *) noexcept;

private:
  local_sink() noexcept = default;
  ~local_sink() noexcept;

  local_sink(local_sink const &) = delete;
  local_sink &operator=(local_sink const &) = delete;
  local_sink(local_sink &&) = delete;
  local_sink &operator=(local_sink &&) = delete;

  void allocate_next_records();

  records *first{nullptr}, *last{nullptr};
  bool active{false};
};

} // namespace rsm::impl

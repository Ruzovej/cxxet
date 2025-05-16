#pragma once

#include "impl/central_sink.hpp"
#include "impl/record.hpp"
#include "impl/records.hpp"

namespace rsm::impl {

struct thread_local_sink {
  static void init();

  [[nodiscard]] static thread_local_sink *instance() noexcept;

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
  thread_local_sink() noexcept = default;
  ~thread_local_sink() noexcept;

  thread_local_sink(thread_local_sink const &) = delete;
  thread_local_sink &operator=(thread_local_sink const &) = delete;
  thread_local_sink(thread_local_sink &&) = delete;
  thread_local_sink &operator=(thread_local_sink &&) = delete;

  void allocate_next_records();

  records *first{nullptr}, *last{nullptr};
  bool active{false};
};

} // namespace rsm::impl

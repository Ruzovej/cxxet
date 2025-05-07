#pragma once

#include "impl/records.hpp"

namespace rsm::impl {

struct thread {
  static void init(unsigned const block_size);

  [[nodiscard]] static inline thread *instance() noexcept {
    thread_local thread t;
    return &t;
  }
  
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

  void flush_to_global() noexcept;

private:
  thread() = default;
  ~thread() noexcept;

  thread(thread const &) = delete;
  thread &operator=(thread const &) = delete;
  thread(thread &&) = delete;
  thread &operator=(thread &&) = delete;

  void allocate_next_records();

  records *first{nullptr}, *last{nullptr};

  unsigned block_size{};
  bool active{false};
};

} // namespace rsm::impl

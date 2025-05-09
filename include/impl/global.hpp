#pragma once

#include <mutex>

#include "impl/records.hpp"
#include "rsm_output_format.hpp"

namespace rsm::impl {

struct global {
  [[nodiscard]] static global *instance() noexcept;

  void append(records *recs) noexcept;

  void dump_and_deallocate_collected_records(output::format const fmt, char const *const filename);

  [[nodiscard]] unsigned get_block_size() const noexcept { return block_size; }

private:
  global();
  ~global() noexcept;

  void deallocate_current() noexcept;

  global(global const &) = delete;
  global &operator=(global const &) = delete;
  global(global &&) = delete;
  global &operator=(global &&) = delete;

  std::mutex mtx;
  records *first{nullptr}, *last{nullptr};
  long long const time_point;
  unsigned const block_size;
};

} // namespace rsm::impl

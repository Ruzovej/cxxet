#pragma once

#include "impl/records.hpp"

namespace rsm::impl {

struct global {
  [[nodiscard]] static global *instance() noexcept;

  void append(records *recs) noexcept;

  void print_records() const;

  [[nodiscard]] unsigned get_block_size() const noexcept { return block_size; }

private:
  global();
  ~global() noexcept;

  global(global const &) = delete;
  global &operator=(global const &) = delete;
  global(global &&) = delete;
  global &operator=(global &&) = delete;

  records *first{nullptr}, *last{nullptr};
  unsigned block_size{64}; // Default block size
};

} // namespace rsm::impl

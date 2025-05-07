#pragma once

#include "impl/records.hpp"

namespace rsm::impl {

struct global {
  [[nodiscard]] static global *instance() noexcept;

  void append(records *recs) noexcept;

  void print_records() const;

private:
  global() = default;
  ~global() noexcept;

  global(global const &) = delete;
  global &operator=(global const &) = delete;
  global(global &&) = delete;
  global &operator=(global &&) = delete;

  records *first{nullptr}, *last{nullptr};
};

} // namespace rsm::impl

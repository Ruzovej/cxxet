#pragma once

#include <mutex>

#include "impl/records.hpp"
#include "rsm_output_format.hpp"

namespace rsm::impl {

struct central_sink {
  [[nodiscard]] static central_sink *instance() noexcept;

  void append(records *recs) noexcept;

  void dump_and_deallocate_collected_records(output::format const fmt,
                                             char const *const filename);

  [[nodiscard]] unsigned get_block_size() const noexcept { return block_size; }

private:
  central_sink();
  ~central_sink() noexcept;

  void deallocate_current() noexcept;

  central_sink(central_sink const &) = delete;
  central_sink &operator=(central_sink const &) = delete;
  central_sink(central_sink &&) = delete;
  central_sink &operator=(central_sink &&) = delete;

  std::mutex mtx;
  records *first{nullptr}, *last{nullptr};
  long long const time_point;
  unsigned const block_size;
};

} // namespace rsm::impl

#pragma once

#include <memory>
#include <mutex>

#include "impl/records.hpp"
#include "rsm_output_format.hpp"

namespace rsm::impl {

struct central_sink {
  central_sink();
  ~central_sink() noexcept = default;

  void append(std::unique_ptr<records, typename records::RecordsDeleter>
                  &&recs) noexcept;

  void dump_and_deallocate_collected_records(output::format const fmt,
                                             char const *const filename);

  [[nodiscard]] unsigned get_block_size() const noexcept { return block_size; }

private:
  central_sink(central_sink const &) = delete;
  central_sink &operator=(central_sink const &) = delete;
  central_sink(central_sink &&) = delete;
  central_sink &operator=(central_sink &&) = delete;

  std::mutex mtx;
  std::unique_ptr<records, typename records::RecordsDeleter> first{nullptr};
  records *last{nullptr};
  long long const time_point;
  unsigned const block_size;
};

} // namespace rsm::impl

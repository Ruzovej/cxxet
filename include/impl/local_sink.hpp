#pragma once

#include <memory>

#include "impl/central_sink.hpp"
#include "impl/record.hpp"
#include "impl/records.hpp"

namespace rsm::impl {

struct local_sink {
  explicit local_sink(central_sink *aParent);
  ~local_sink() noexcept;

  inline void append_record(record const m) {
    if (!last || !last->free_capacity()) // [[unlikely]] // TODO ...
    {
      allocate_next_records();
    }
    last->append_record(m);
  }

  void flush_to_parent_sink() noexcept;

  void set_parent_sink(central_sink *aParent) noexcept;

private:
  local_sink(local_sink const &) = delete;
  local_sink &operator=(local_sink const &) = delete;
  local_sink(local_sink &&) = delete;
  local_sink &operator=(local_sink &&) = delete;

  void allocate_next_records();

  central_sink *parent{nullptr};
  std::unique_ptr<records, typename records::RecordsDeleter> first{nullptr};
  records *last{nullptr};
};

} // namespace rsm::impl

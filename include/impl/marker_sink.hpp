#pragma once

#include <mutex>
#include <optional>

#include "impl/record.hpp"
#include "impl/records.hpp"
#include "rsm_output_format.hpp"

namespace rsm::impl {

// Forward declaration for the parent pointer
class marker_sink;

/**
 * A generalized class to handle marker data collection
 * Can be used either as a thread-local sink or as a global sink
 */
class marker_sink {
public:
  // Global instance accessor
  [[nodiscard]] static marker_sink* global_instance() noexcept;
  
  // Thread-local instance accessor
  [[nodiscard]] static marker_sink* thread_instance() noexcept;
  
  // Initialize the thread-local sink (needs to be called once per thread)
  static void init_thread();
  
  // Create a marker_sink with optional parent (global has no parent)
  explicit marker_sink(marker_sink* parent = nullptr);
  ~marker_sink() noexcept;
  
  // Append a single record to this sink
  inline void append_record(record const m) {
    if (active) {
      if (!last || !last->free_capacity()) {
        allocate_next_records();
      }
      last->append_record(m);
    }
  }
  
  // Append a chain of records to this sink (used when another sink flushes to this one)
  void append(records* recs) noexcept;
  
  // Flush this sink's records to its parent (if any)
  void flush_to_parent() noexcept;
  
  // Dump all records and deallocate them (typically used from global instance)
  void dump_and_deallocate_collected_records(
      output::format const fmt, 
      char const* const filename);
  
  // Get the configured block size
  [[nodiscard]] unsigned get_block_size() const noexcept { return block_size; }

private:
  // Allocate a new records block and update pointers
  void allocate_next_records();
  
  // Deallocate all records in this sink
  void deallocate_current() noexcept;
  
  // No copying or moving
  marker_sink(marker_sink const&) = delete;
  marker_sink& operator=(marker_sink const&) = delete;
  marker_sink(marker_sink&&) = delete;
  marker_sink& operator=(marker_sink&&) = delete;

  // Parent sink to flush to (null for global instance)
  marker_sink* parent_sink{nullptr};
  
  // Linked list of record blocks
  records* first{nullptr};
  records* last{nullptr};
  
  // Whether this sink is actively collecting records
  bool active{false};
  
  // For the global instance
  std::mutex mtx;
  long long time_point{0};
  unsigned block_size{64};
};

} // namespace rsm::impl
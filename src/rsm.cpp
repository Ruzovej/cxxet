#include "rsm.hpp"

#include "impl/central_sink.hpp"
#include "impl/local_sink.hpp"
#include "impl/sink_properties.hpp"

static rsm::impl::sink_properties sink_props{};
static rsm::impl::central_sink global_sink{sink_props};
static thread_local rsm::impl::local_sink thread_sink{global_sink};

void RSM_init_thread_local_sink() noexcept {
  // if not already initialized, preallocates memory
  (void)thread_sink;
}

void RSM_thread_local_sink_reserve(int const minimum_free_capacity) noexcept {
  if (minimum_free_capacity > 0) {
    thread_sink.reserve(minimum_free_capacity);
  }
}

void RSM_flush_thread_local_sink() noexcept { thread_sink.flush(); }

void RSM_flush_all_collected_events(rsm::output::format const fmt,
                                    char const *const filename,
                                    bool const defer_flush) noexcept {
  sink_props.set_target_format(fmt).set_target_filename(filename);
  if (!defer_flush) {
    global_sink.flush();
  }
}

void RSM_IMPL_append_event(rsm::impl::event::any const &evt) noexcept {
  thread_sink.append_event(evt);
}

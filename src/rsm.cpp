#include "rsm.hpp"

#include "impl/central_sink.hpp"
#include "impl/local_sink.hpp"
#include "impl/sink_properties.hpp"

namespace rsm {

static impl::sink_properties sink_props{};
static impl::central_sink global_sink{sink_props};
static thread_local impl::local_sink thread_sink{global_sink};

void init_thread_local_sink() {
  // if not already initialized, preallocates memory
  thread_sink.reserve();
}

void flush_thread_local_sink() noexcept { thread_sink.flush(); }

void flush_all_collected_events(output::format const fmt,
                                char const *const filename,
                                bool const defer_flush) {
  sink_props.set_target_format(fmt).set_target_filename(filename);
  if (!defer_flush) {
    global_sink.flush();
  }
}

void marker::append_event(impl::event::any const &evt) noexcept {
  thread_sink.append_event(evt);
}

} // namespace rsm

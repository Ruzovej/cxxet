#include "rsm.hpp"

#include <cassert>

#include "impl/central_sink.hpp"
#include "impl/local_sink.hpp"

namespace rsm {

static impl::central_sink global_sink{false};
static thread_local impl::local_sink thread_sink{global_sink};

void init_thread_local_sink() {
  // if not already initialized, preallocates memory
  thread_sink.reserve();
}

void flush_thread_local_sink() noexcept { thread_sink.flush(); }

void flush_all_collected_events(output::format const fmt,
                                char const *const filename,
                                bool const defer_flush) {
  global_sink.set_target_format(fmt).set_target_filename(filename);
  if (!defer_flush) {
    global_sink.flush();
  }
}

void marker::append_event(impl::event::any const &evt) noexcept {
  thread_sink.append_event(evt);
}

} // namespace rsm

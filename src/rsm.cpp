#include "rsm.hpp"

#include <cassert>

#include <iostream> // must be there, even if unused ... so `std::cout`, etc. gets initialized before it's potentially used in `sink_properties` ctor

#include "impl/central_sink.hpp"
#include "impl/local_sink.hpp"
#include "impl/sink_properties.hpp"

static rsm::impl::sink_properties sink_props{};
static rsm::impl::central_sink global_sink{sink_props};
static thread_local rsm::impl::local_sink thread_sink{global_sink};
#ifndef NDEBUG
static thread_local bool was_initialized{false};
#endif

void RSM_init_thread_local_sink() noexcept {
#ifndef NDEBUG
  assert(!was_initialized &&
         "RSM_init_thread_local_sink() called multiple times");
  was_initialized = true;
#endif

  thread_sink.reserve();
}

void RSM_thread_local_sink_reserve(int const minimum_free_capacity) noexcept {
#ifndef NDEBUG
  was_initialized = true;
#endif
  thread_sink.reserve(minimum_free_capacity);
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

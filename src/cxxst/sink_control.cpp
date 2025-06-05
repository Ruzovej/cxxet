#include "cxxst/sink_control.hpp"
#include "impl/thread_local_sink_submit_event.hpp"

#include <cassert>

#include <iostream> // must be there, even if unused ... so `std::cout`, etc. gets initialized before it's potentially used in `sink_properties` ctor

#include "impl/central_sink.hpp"
#include "impl/local_sink.hpp"
#include "impl/sink_properties.hpp"

namespace cxxst {

static impl::sink_properties sink_props{};
static impl::central_sink global_sink{sink_props};
static thread_local impl::local_sink thread_sink{&global_sink};
#ifndef NDEBUG
static thread_local bool was_initialized{false};
#endif

void init_thread_local_sink() noexcept {
#ifndef NDEBUG
  assert(!was_initialized &&
         "CXXST_init_thread_local_sink() called multiple times");
  was_initialized = true;
#endif

  thread_sink.reserve(sink_props.default_list_node_capacity);
}

void thread_local_sink_reserve(int const minimum_free_capacity) noexcept {
#ifndef NDEBUG
  was_initialized = true;
#endif
  thread_sink.reserve(minimum_free_capacity <= 0
                          ? sink_props.default_list_node_capacity
                          : minimum_free_capacity);
}

void flush_thread_local_sink() noexcept { thread_sink.flush(); }

void flush_global_sink(cxxst::output::format const fmt,
                       char const *const filename,
                       bool const defer_flush) noexcept {
  sink_props.set_target_format(fmt).set_target_filename(filename);
  if (!defer_flush) {
    global_sink.flush();
  }
}

namespace impl {

void thread_local_sink_submit_event(event::any const &evt) noexcept {
  thread_sink.append_event(evt);
}

} // namespace impl
} // namespace cxxst

#include "cxxst/sink_control.hpp"
#include "impl/thread_local_sink_submit_event.hpp"

#include <cassert>

#include <iostream> // must be there, even if unused ... so `std::cout`, etc. gets initialized before it's potentially used in `sink_properties` ctor
#include <optional>

#include "impl/central_sink.hpp"
#include "impl/local_sink.hpp"
#include "impl/sink_properties.hpp"

namespace cxxst {

namespace {
impl::sink_properties const sink_props{};
impl::central_sink global_sink{sink_props};
thread_local std::optional<impl::local_sink> thread_sink;
} // namespace

void sink_thread_reserve(int const minimum_free_capacity) noexcept {
  if (thread_sink == std::nullopt) {
    thread_sink.emplace(&global_sink);
  }
  thread_sink->reserve(minimum_free_capacity <= 0
                           ? sink_props.default_list_node_capacity
                           : minimum_free_capacity);
}

void sink_thread_flush() noexcept {
  assert(thread_sink != std::nullopt && "thread local sink not initialized!");
  thread_sink->flush();
}

void sink_global_flush(cxxst::output::format const fmt,
                       char const *const filename,
                       bool const defer_flush) noexcept {
  global_sink.flush(fmt, filename, defer_flush);
}

namespace impl {

void thread_local_sink_submit_event(event::any const &evt) noexcept {
  assert(thread_sink != std::nullopt && "thread local sink not initialized!");
  thread_sink->append_event(evt);
}

} // namespace impl
} // namespace cxxst

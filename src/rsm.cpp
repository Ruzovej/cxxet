#include "rsm.hpp"

#include "impl/central_sink.hpp"
#include "impl/local_sink.hpp"

namespace rsm {

static impl::central_sink global_sink{false};
static thread_local impl::local_sink thread_sink{&global_sink};

void init_local_sink(impl::sink *parent_sink, int const default_node_capacity) {
  // if not already initialized, preallocates memory & resets parent
  thread_sink.set_parent_sink(parent_sink ? parent_sink : &global_sink)
      .set_default_list_node_capacity(
          default_node_capacity
              ? default_node_capacity
              : (parent_sink ? parent_sink->get_default_capacity()
                             : global_sink.get_default_capacity()))
      .reserve();
}

void flush_thread() noexcept { thread_sink.flush_to_parent(); }

void dump_collected_records(output::format const fmt,
                            char const *const filename) {
  global_sink.set_target_format(fmt).set_target_filename(filename).flush();
}

void marker::append_event(impl::event::any const &evt) noexcept {
  thread_sink.append_event(evt);
}

} // namespace rsm

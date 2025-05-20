#include "rsm.hpp"

#include "impl/central_sink.hpp"
#include "impl/local_sink.hpp"
#include "impl/record.hpp"

namespace rsm {

static impl::central_sink global_sink{};
static thread_local impl::local_sink thread_sink{&global_sink};

void init_local_sink(impl::central_sink *parent_sink) {
  // if not already initialized, preallocates memory & resets parent
  thread_sink.set_parent_sink(parent_sink ? parent_sink : &global_sink);
}

void flush_thread() noexcept { thread_sink.flush_to_parent_sink(); }

void dump_collected_records(output::format const fmt,
                            char const *const filename) {
  global_sink.dump_and_deallocate_collected_records(fmt, filename);
}

void marker::append_record(long long const start_ns,
                           long long const end_ns) noexcept {
  thread_sink.append_record({desc, color, tag, start_ns, end_ns});
}

} // namespace rsm

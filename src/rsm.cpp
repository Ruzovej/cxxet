#include "rsm.hpp"

#include "impl/marker_sink.hpp"
#include "impl/record.hpp"

namespace rsm {

void init_thread() { impl::marker_sink::init_thread(); }

void flush_thread() noexcept { impl::marker_sink::thread_instance()->flush_to_parent(); }

void dump_collected_records(output::format const fmt,
                            char const *const filename) {
  impl::marker_sink::global_instance()->dump_and_deallocate_collected_records(fmt, filename);
}

void marker::append_record(long long const start_ns,
                           long long const end_ns) noexcept {
  impl::marker_sink::thread_instance()->append_record({desc, color, tag, start_ns, end_ns});
}

} // namespace rsm

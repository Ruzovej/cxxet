#include "rsm.hpp"

#include "impl/global.hpp"
#include "impl/record.hpp"
#include "impl/thread_local_sink.hpp"

namespace rsm {

void init_thread_local_sink() { impl::thread_local_sink::init(); }

void flush_thread() noexcept {
  impl::thread_local_sink::instance()->flush_to_global();
}

void dump_collected_records(output::format const fmt,
                            char const *const filename) {
  impl::global::instance()->dump_and_deallocate_collected_records(fmt,
                                                                  filename);
}

void marker::append_record(long long const start_ns,
                           long long const end_ns) noexcept {
  impl::thread_local_sink::instance()->append_record(
      {desc, color, tag, start_ns, end_ns});
}

} // namespace rsm

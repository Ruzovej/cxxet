#include "rsm.hpp"

#include "impl/global.hpp"
#include "impl/record.hpp"
#include "impl/thread.hpp"

namespace rsm {

void init_thread() { impl::thread::init(); }

void flush_thread() noexcept { impl::thread::instance()->flush_to_global(); }

void dump_collected_records() { rsm::impl::global::instance()->print_records(); }

void marker::append_record(long long const start_ns,
                           long long const end_ns) noexcept {
  impl::thread::instance()->append_record({desc, color, tag, start_ns, end_ns});
}

} // namespace rsm

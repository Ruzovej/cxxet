#include "rsm.hpp"

#include "impl/global.hpp"
#include "impl/record.hpp"
#include "impl/thread.hpp"

namespace rsm {

void init_thread(unsigned const block_size) { impl::thread::init(block_size); }

void append_record(char const *desc, long long const start_ns,
                   long long const end_ns) noexcept {
  impl::thread::instance()->append_record(impl::record{desc, start_ns, end_ns});
}

void flush_thread() noexcept { impl::thread::instance()->flush_to_global(); }

void print_flushed_records() { rsm::impl::global::instance()->print_records(); }

} // namespace rsm

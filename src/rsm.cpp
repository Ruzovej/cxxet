#include "rsm.hpp"

#include "impl/global.hpp"
#include "impl/thread.hpp"

namespace rsm {

void init_thread(unsigned const block_size) { impl::thread::init(block_size); }

void flush_thread() noexcept { impl::thread::instance()->flush_to_global(); }

void print_flushed_records() { rsm::impl::global::instance()->print_records(); }

} // namespace rsm

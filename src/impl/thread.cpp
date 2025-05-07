#include "impl/thread.hpp"

#include <cassert>

#include "impl/global.hpp"

namespace rsm::impl {

void thread::init() {
  [[maybe_unused]] auto const global_inst{
      global::instance()}; // ensure global is initialized before (and hence
                           // destroyed after) any thread_local instance
  auto const inst{instance()};
  assert(!inst->active &&
         "calling thread::init more than once in the current thread!");
  if (!inst->active) {
    inst->block_size = global::instance()->get_block_size();
    inst->active = true;
    inst->allocate_next_records();
  }
}

void thread::flush_to_global() noexcept {
  if (active) {
    global::instance()->append(first);
  }
  first = last = nullptr;
  active = false;
}

thread::~thread() noexcept { flush_to_global(); }

void thread::allocate_next_records() {
  auto target{last ? &last->next : &last};
  *target = new records(block_size);
  if (!first) {
    first = *target;
  }
}

} // namespace rsm::impl

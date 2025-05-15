#include "impl/thread_local_sink.hpp"

#include <cassert>

#include "impl/global.hpp"

namespace rsm::impl {

void thread_local_sink::init() {
  [[maybe_unused]] auto const global_inst{
      global::instance()}; // ensure global is initialized before (and hence
                           // destroyed after) any thread_local instance
  auto const inst{instance()};
  assert(!inst->active &&
         "calling thread::init more than once in the current thread!");
  if (!inst->active) {
    inst->active = true;
    inst->allocate_next_records();
  }
}

void thread_local_sink::flush_to_global() noexcept {
  if (active) {
    global::instance()->append(first);
    first = last = nullptr;
    active = false;
  }
}

thread_local_sink::~thread_local_sink() noexcept { flush_to_global(); }

void thread_local_sink::allocate_next_records() {
  // [[assume((first == nullptr) == (last == nullptr))]];
  auto target{first ? &last->next : &last};
  *target = new records(global::instance()->get_block_size());
  if (!first) {
    first = *target;
  } else {
    last = *target;
  }
}

} // namespace rsm::impl

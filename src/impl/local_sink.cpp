#include "impl/local_sink.hpp"

#include <cassert>

#include "impl/central_sink.hpp"

namespace rsm::impl {

void local_sink::init() {
  [[maybe_unused]] auto const global_inst{
      central_sink::instance()}; // ensure global is initialized before (and
                                 // hence destroyed after) any thread_local
                                 // instance
  auto const inst{instance()};
  assert(!inst->active &&
         "calling thread::init more than once in the current thread!");
  if (!inst->active) {
    inst->active = true;
    inst->allocate_next_records();
  }
}

[[nodiscard]] local_sink *local_sink::instance() noexcept {
  thread_local local_sink t;
  return &t;
}

void local_sink::flush_to_central_sink(central_sink *sink) noexcept {
  if (active) {
    sink->append(first);
    first = last = nullptr;
    active = false;
  }
}

local_sink::~local_sink() noexcept {
  flush_to_central_sink(central_sink::instance());
}

void local_sink::allocate_next_records() {
  // [[assume((first == nullptr) == (last == nullptr))]];
  auto target{first ? &last->next : &last};
  *target = new records(central_sink::instance()->get_block_size());
  if (!first) {
    first = *target;
  } else {
    last = *target;
  }
}

} // namespace rsm::impl

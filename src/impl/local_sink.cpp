#include "impl/local_sink.hpp"

#include <cassert>

#include "impl/central_sink.hpp"

namespace rsm::impl {

local_sink::local_sink(central_sink *aParent) : parent{aParent} {
  assert(parent && "attempting to create a local sink without a parent");
  allocate_next_records();
}

local_sink::~local_sink() noexcept { flush_to_parent_sink(); }

void local_sink::flush_to_parent_sink() noexcept {
  if (first) {
    parent->append(std::move(first));
    last = nullptr;
  }
}

void local_sink::set_parent_sink(central_sink *aParent) noexcept {
  assert((!first || first->empty()) &&
         "attempting to set a parent sink with unflushed records");
  parent = aParent;
}

void local_sink::allocate_next_records() {
  // [[assume((first == nullptr) == (last == nullptr))]];
  auto target{first ? &last->next : &first};
  *target = records::create(parent->get_block_size());

  if (!last) {
    last = first.get();
  } else {
    last = last->next.get();
  }
}

} // namespace rsm::impl

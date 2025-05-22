#include "impl/sink.hpp"

#include <cassert>

namespace rsm::impl {

sink::sink(sink *aParent, int const default_list_node_capacity)
    : parent{aParent} {
  assert(default_list_node_capacity > 0);
  events.set_default_node_capacity(default_list_node_capacity);
}

sink::~sink() noexcept {
  flush();
}

void sink::initialize(sink *aParent,
                      int const default_list_node_capacity) noexcept {
  assert(aParent != this);
  parent = aParent;
  assert(default_list_node_capacity > 0);
  events.set_default_node_capacity(default_list_node_capacity);
  events.reserve();
}

} // namespace rsm::impl

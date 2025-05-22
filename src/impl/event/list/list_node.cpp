#include "impl/event/list/list_node.hpp"

#include <unistd.h>

#include <cassert>

#include <type_traits>
#include <utility>

namespace rsm::impl::event::list_node {

static_assert(std::is_trivially_destructible_v<meta_info>);
static_assert(std::is_trivially_destructible_v<raw_element>);

handler::handler() noexcept = default;

handler::~handler() noexcept { destroy(); }

void handler::destroy() noexcept {
  while (first) {
    auto *next{first->meta.next};
    delete[] first;
    first = next;
  }
}

handler &handler::set_default_capacity(int const capacity) noexcept {
  assert(capacity > 0);
  default_capacity = capacity;
  return *this;
}

void handler::drain_other(handler &other) noexcept {
  assert(this != &other && "attempting to drain and append to self!");
  assert((first == nullptr) == (last == nullptr));
  assert((other.first == nullptr) == (other.last == nullptr));
  if (last) {
    last[0].meta.next = std::exchange(other.first, nullptr);
    while (last[0].meta.next) {
      last = last[0].meta.next;
    }
    other.last = nullptr;
  } else {
    std::swap(first, other.first);
    std::swap(last, other.last);
  }
}

[[nodiscard]] bool handler::empty() const noexcept {
  if (first != nullptr) {
    for (auto it{first}; it != nullptr; it = it[0].meta.next) {
      if (it[0].meta.size > 0) {
        return false;
      }
    }
  }

  return true;
}

[[nodiscard]] long long handler::size() const noexcept {
  long long sz{0};

  for (auto it{first}; it != nullptr; it = it[0].meta.next) {
    sz += it[0].meta.size;
  }

  return sz;
}

static raw_element *allocate_raw_node_elems(int const capacity) noexcept {
  // this can throw ... but if it does, it means allocation failed -> how to
  // handle it? Let's just crash ...
  auto *data{new raw_element[static_cast<unsigned>(capacity) + 1]};

  new (&data[0].meta)
      meta_info{static_cast<long long>(gettid()), nullptr, 0, capacity};

  return data;
}

long long handler::get_pid() const noexcept {
  return static_cast<long long>(getpid());
}

void handler::do_reserve(bool const force) noexcept {
  if (force || (get_current_free_capacity() < default_capacity)) {
    auto target{first ? &last->meta.next : &first};
    *target = allocate_raw_node_elems(default_capacity);
    last = *target;
  }
}

} // namespace rsm::impl::event::list_node

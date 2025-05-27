#include "impl/event/list/list.hpp"

#include <unistd.h>

#include <cassert>

#include <type_traits>
#include <utility>

namespace rsm::impl::event {

namespace {

list::raw_element *allocate_raw_node_elems(int const capacity) noexcept {
  // this can throw ... but if it does, it means allocation failed -> how to
  // handle it? Let's just crash ...
  auto *data{new list::raw_element[static_cast<unsigned>(capacity) + 1]};

  new (&data[0].meta)
      list::meta_info{static_cast<long long>(gettid()), nullptr, 0, capacity};

  return data;
}

} // namespace

list::list() noexcept = default;

list::~list() noexcept {
  static_assert(std::is_trivially_destructible_v<list::meta_info>);
  static_assert(std::is_trivially_destructible_v<list::raw_element>);
  destroy();
}

void list::destroy() noexcept {
  while (first) {
    auto *next{first->meta.next};
    delete[] first;
    first = next;
  }
}

void list::set_default_node_capacity(int const capacity) noexcept {
  assert(capacity > 0);
  default_capacity = capacity;
}

void list::reserve(bool const force) noexcept {
  if (force || (get_current_free_capacity() < default_capacity)) {
    auto target{first ? &last[0].meta.next : &first};
    *target = allocate_raw_node_elems(default_capacity);
    last = *target;
  }
}

void list::drain_other(list &other) noexcept {
  assert(this != &other && "attempting to drain and append to self!");
  assert((first == nullptr) == (last == nullptr));
  assert((other.first == nullptr) == (other.last == nullptr));
  if (last) {
    last[0].meta.next = std::exchange(other.first, nullptr);
    last = std::exchange(other.last, nullptr);
  } else {
    std::swap(first, other.first);
    std::swap(last, other.last);
  }
}

[[nodiscard]] bool list::empty() const noexcept {
  if (first != nullptr) {
    for (auto it{first}; it != nullptr; it = it[0].meta.next) {
      if (it[0].meta.size > 0) {
        return false;
      }
    }
  }
  return true;
}

[[nodiscard]] long long list::size() const noexcept {
  long long sz{0};
  for (auto it{first}; it != nullptr; it = it[0].meta.next) {
    sz += it[0].meta.size;
  }
  return sz;
}

long long list::get_pid() noexcept { return static_cast<long long>(getpid()); }

} // namespace rsm::impl::event

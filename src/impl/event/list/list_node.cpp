#include "impl/event/list/list_node.hpp"

#include <unistd.h>

#include <cassert>

#include <type_traits>
#include <utility>

namespace rsm::impl::event::list_node {

static_assert(std::is_trivially_destructible_v<meta_info>);
static_assert(std::is_trivially_destructible_v<raw_element>);

handler::handler() noexcept = default;

handler::~handler() noexcept {
  while (first) {
    auto *next{first->meta.next};
    delete[] first;
    first = next;
  }
}

void handler::drain_and_prepend_other(handler &other) noexcept {
  assert(this != &other && "attempting to drain and prepend self");
  if (other.first) {
    other.last[0].meta.next = first;
    first = std::exchange(other.first, nullptr);
    other.last = nullptr;
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
  assert(capacity >= 1);

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

#include "rsm.hpp"

#include <unistd.h>

#include <cassert>

#include <iostream>
#include <mutex>
#include <new>
#include <thread>

namespace rsm::impl {

records::records(unsigned const block_size)
    : thread_id{static_cast<unsigned long long>(gettid())},
      first{new record[block_size]}, last{first}, capacity{first + block_size} {
}

records::~records() noexcept { delete[] first; }

void records::print_records() const {
  for (auto iter{first}; iter < last; ++iter) {
    std::cout << thread_id << ": '" << iter->desc << "' " << iter->start_ns
              << " -> " << iter->end_ns << " ~ "
              << (iter->end_ns - iter->start_ns) << " [ns]\n";
  }
}

namespace {
std::mutex global_access_mutex{};
}

[[nodiscard]] global *global::instance() noexcept {
  static global g;
  return &g;
}

void global::append(records *recs) noexcept {
  std::lock_guard lck{global_access_mutex};
  if (!first) {
    first = recs;
  } else {
    last->next = recs;
  }
  while (recs->next) {
    recs = recs->next;
  }
  last = recs;
}

void global::print_records() const {
  for (auto iter{first}; iter != nullptr; iter = iter->next) {
    iter->print_records();
  }
}

global::~global() noexcept {
  while (first) {
    auto const next{first->next};
    delete first;
    first = next;
  }
}

void thread::init(unsigned const block_size) {
  auto const inst{instance()};
  assert(!inst->active &&
         "calling thread::init more than once in the current thread!");
  if (!inst->active) {
    inst->block_size = block_size;
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

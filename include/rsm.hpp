#pragma once

#if defined(_WIN32)
#include <windows.h>
#error "Unimplemented platform - TODO ..."
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#include <time.h>
#else
#error "Unsupported platform"
#endif

namespace rsm {

namespace impl {

[[nodiscard]] inline long long now_ns() {
  struct timespec t;
  clock_gettime(CLOCK_MONOTONIC, &t);
  static_assert(sizeof(long long) < sizeof(struct timespec),
                "conversion to `long long` doesn't make sense");
  return static_cast<long long>(t.tv_sec * 1'000'000'000 + t.tv_nsec);
}

}; // namespace impl

struct marker {
  inline marker(char const *aDesc) : desc{aDesc}, start_ns{impl::now_ns()} {}
  inline ~marker() noexcept { submit(); }

  [[maybe_unused]] inline long long submit() noexcept;

private:
  char const *desc;
  long long const start_ns;
};

namespace impl {

struct record {
  char const *desc;
  long long start_ns, end_ns;
};

struct records {
  records(
      unsigned const
          block_size); // TODO optimize this ... don't allocate `first` on heap,
                       // maybe allocate this whole structure via `malloc` with
                       // extra space for the data & set the pointers to it?
  ~records() noexcept;

  [[nodiscard]] inline bool free_capacity() const noexcept {
    return last < capacity;
  }

  inline void append_record(record const r) noexcept { *(last++) = r; }

  void print_records() const;

  unsigned long long const thread_id;
  records *next{nullptr};
  record *const first, *last, *const capacity;
};

struct global {
  [[nodiscard]] static global *instance() noexcept;

  void append(records *recs) noexcept;

  void print_records() const;

private:
  global() = default;
  ~global() noexcept;

  records *first{nullptr}, *last{nullptr};
};

struct thread {
  static void init(unsigned const block_size);

  [[nodiscard]] static inline thread *instance() noexcept {
    thread_local thread t;
    return &t;
  }
  inline void append_record(record const m) {
    if (active) // [[likely]] // TODO ...
    {
      if (!last || !last->free_capacity()) // [[unlikely]] // TODO ...
      {
        allocate_next_records();
      }
      last->append_record(m);
    }
  }

  void flush_to_global() noexcept;

private:
  thread() = default;
  ~thread() noexcept;

  void allocate_next_records();

  records *first{nullptr}, *last{nullptr};

  unsigned block_size{};
  bool active{false};
};

} // namespace impl

[[maybe_unused]] inline long long marker::submit() noexcept {
  if (desc) // [[likely]] // TODO
  {
    auto const now{impl::now_ns()};
    impl::thread::instance()->append_record(impl::record{desc, start_ns, now});
    desc = nullptr;
    return now - start_ns;
  }
  return -1;
}

static inline void init_thread(unsigned const block_size) {
  impl::thread::init(block_size);
}

static inline void flush_thread() noexcept {
  impl::thread::instance()->flush_to_global();
}

} // namespace rsm

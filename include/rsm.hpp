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

[[nodiscard]] inline struct timespec now() noexcept {
  // https://stackoverflow.com/a/42658433
  // https://www.man7.org/linux/man-pages/man3/clock_gettime.3.html
  constexpr int clock_type{
      // choose exactly one of:
      // CLOCK_MONOTONIC // sometimes higher latency
      // CLOCK_MONOTONIC_COARSE // dosn't work (on my primary PC)
      // CLOCK_BOOTTIME // "equivalent" to `CLOCK_MONOTONIC`
      // CLOCK_THREAD_CPUTIME_ID // very low resolution, seems unusable
      CLOCK_MONOTONIC_RAW // seems best
  };
  struct timespec t;
  clock_gettime(clock_type, &t);
  return t;
}

[[nodiscard]] inline long long as_int_ns(struct timespec const t) noexcept {
  return static_cast<long long>(t.tv_sec * 1'000'000'000 + t.tv_nsec);
}

}; // namespace impl

struct marker {
  inline marker(char const *aDesc) : desc{aDesc}, start{impl::now()} {}
  inline ~marker() noexcept { submit(); }

  [[maybe_unused]] inline long long submit() noexcept;

private:
  char const *desc;
  struct timespec const start;
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

  global(global const &) = delete;
  global &operator=(global const &) = delete;
  global(global &&) = delete;
  global &operator=(global &&) = delete;

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

  thread(thread const &) = delete;
  thread &operator=(thread const &) = delete;
  thread(thread &&) = delete;
  thread &operator=(thread &&) = delete;

  void allocate_next_records();

  records *first{nullptr}, *last{nullptr};

  unsigned block_size{};
  bool active{false};
};

} // namespace impl

[[maybe_unused]] inline long long marker::submit() noexcept {
  if (desc) // [[likely]] // TODO
  {
    auto const now_ns{impl::as_int_ns(impl::now())};
    auto const start_ns{impl::as_int_ns(start)};
    impl::thread::instance()->append_record(
        impl::record{desc, start_ns, now_ns});
    desc = nullptr;
    return now_ns - start_ns;
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

#pragma once

#include "impl/event/any.hpp"
#include "impl/utils.hpp"
#include "rsm_output_format.hpp"

#define RSM_MARKER(...)                                                        \
  ::rsm::marker RSM_IMPL_IMPLICIT_MARKER_NAME(RSM_IMPLICIT_MARKER_,            \
                                              __LINE__) {                      \
    __VA_ARGS__                                                                \
  }

void RSM_init_thread_local_sink() noexcept;

void RSM_flush_thread_local_sink() noexcept;

void RSM_flush_all_collected_events(
    rsm::output::format const fmt = rsm::output::format::chrome_trace,
    char const *const filename = nullptr, // `== nullptr` => no-op; to be more
                                          // precise: discard everything
    bool const defer_flush = false) noexcept;

namespace rsm {

struct marker {
  inline marker(char const *aDesc, int const aColor = -1,
                int const aTag = -1) noexcept
      : desc{aDesc}, color{aColor}, tag{aTag}, start{impl::now()} {}
  inline ~marker() noexcept { submit(); }

  [[maybe_unused]] inline long long submit() noexcept {
    if (desc) // [[likely]] // TODO
    {
      auto const now_ns{impl::as_int_ns(impl::now())};
      auto const start_ns{impl::as_int_ns(start)};
      append_event(impl::event::complete{static_cast<char>(color),
                                         static_cast<short>(tag), 0, desc,
                                         start_ns, now_ns - start_ns});
      desc = nullptr;
      return now_ns - start_ns;
    }
    return -1;
  }

private:
  char const *desc;
  int const color; // TODO discard those members and ignore them ...
  int const tag;   // TODO discard those members and ignore them ...
  struct timespec const start;

  void append_event(impl::event::any const &evt) noexcept;

  marker(marker const &) = delete;
  marker &operator=(marker const &) = delete;
  marker(marker &&) = delete;
  marker &operator=(marker &&) = delete;
};

} // namespace rsm

#pragma once

#include <tuple>

namespace rsm::impl::event {

// inspired by
// https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU

enum class type : char {
  duration_begin = 'B',
  duration_end = 'E',
  complete = 'X',
  instant = 'i',
  counter = 'C',
  // TODO implement or at least consider:
  // async: 'b' nestable begin, 'n' nestable instant, 'e' nestable end
  // flow: 's' start, 't' step, 'f' end
  unknown = '\0',
};

// https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit?tab=t.0#heading=h.uxpopqvbjezh
template <type bound_type = type::unknown> struct common {
  //                        // related to field:
  type const t{bound_type}; // "ph"
  char flag_1;              // unspecified meaning
  short flag_2;             // unspecified meaning
  int flag_4;               // unspecified meaning
  const char *desc;         // "name"
  // optional (or not?!) fields:
  // * "cat" -> TODO have it here or not?!
  // other mandatory fields:
  // * "pid", "tid" -> provided by the sink, etc.
  // * "ts", "args", ... -> provided by the specific event type

  constexpr common() = default;
  constexpr common(char const *const aDesc) noexcept
      : flag_1{0}, flag_2{0}, flag_4{0}, desc{aDesc} {}
  constexpr common(char const aFlag1, short const aFlag2, int const aFlag4,
                   char const *const aDesc) noexcept
      : flag_1{aFlag1}, flag_2{aFlag2}, flag_4{aFlag4}, desc{aDesc} {}

  [[nodiscard]] constexpr bool operator==(common const &other) const noexcept {
    auto const tie = [](common const &c) {
      return std::tie(c.t, c.flag_1, c.flag_2, c.flag_4, c.desc);
    };
    return tie(*this) == tie(other);
  }
};

} // namespace rsm::impl::event

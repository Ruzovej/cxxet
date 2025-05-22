#pragma once

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
  // async: b nestable begin, n nestable instant, e nestable end
  // flow: s start, t step, f end
};

// https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit?tab=t.0#heading=h.uxpopqvbjezh
struct common {
  type t;                   // "ph"
  char explicit_padding[7]; // TODO provide some meaning to this
  const char *desc;         // "name"
  // optional (or not?!) fields:
  // * "cat" -> TODO have it here or not?!
  // other mandatory fields:
  // * "pid", "tid" -> provided by the sink, etc.
  // * "ts", "args", ... -> provided by the specific event type

  bool operator==(common const &other) const noexcept {
    return t == other.t && desc == other.desc;
  }
};

} // namespace rsm::impl::event

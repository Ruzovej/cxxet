#pragma once

namespace cxxst::output {

enum class format : int {
  chrome_trace =
      0, // https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/preview?tab=t.0
  raw_naive_v0 = 1,
};

} // namespace cxxst::output

#pragma once

namespace rsm::output {

enum class format : int {
  chrome_trace, // https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/preview?tab=t.0
  raw_naive_v0,
  raw_binary_v0,
  raw_json_v0,
};

} // namespace rsm::output

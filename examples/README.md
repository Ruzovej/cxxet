# Examples

This directory contains examples demonstrating the usage of the `cxxst` library for tracing `C++` code.

It's lightweight library that generates `chrome trace` format output, which can be viewed in Chrome's built-in trace viewer (`chrome://tracing`) or tools like [Perfetto UI](https://ui.perfetto.dev/).

## Example categories

* **[complete/](complete/)**
  * Marker for measuring time spans, with begin and end merged into single event.
* **[counter/](counter/)**
  * Marker for tracking changing values over time.
* **[duration/](duration/)**
  * Markers for measuring time spans, with begin and end marked separately.
* **[for_tests/](for_tests/)**
  * Various examples showing how to (not) use provided library.
* **[instant/](instant/)**
  * Marker for point-in-time events.

Examples are executed in acompanying `bats` test suite, while asserting on the output and desired behavior. See [`suite.bats`](../tests/integration/suite/suite.bats) for more details.

## Performance Tips

* Use `CXXST_sink_thread_reserve(capacity)` anytime when you know how many events will be generated; at least preallocate buffer for markers by a call to `CXXST_init_thread_local_sink()` (at most once per thread and before any `CXXST_sink_thread_reserve(...)` call).
* Markers have (low?) overhead that can accumulate within high-frequency usage.

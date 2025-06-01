# Examples

This directory contains examples demonstrating the usage of the `rsm` library for tracing `C++` code.

It's lightweight library that generates `chrome trace` format output, which can be viewed in Chrome's built-in trace viewer (`chrome://tracing`) or tools like [Perfetto UI](https://ui.perfetto.dev/).

## Example categories

* **[instant/](instant/)**
  * Marker for point-in-time events.
* **[counter/](counter/)**
  * Marker for tracking changing values over time.
* **[duration/](duration/)**
  * Markers for measuring time spans, with begin and end marked separately.
* **[complete/](complete/)**
  * Marker for measuring time spans, with begin and end merged into single event.

## Performance Tips

* Use `RSM_thread_local_sink_reserve(capacity)` when you know how many events you'll generate.
* Markers have (low?) overhead that can accumulate within high-frequency usage.

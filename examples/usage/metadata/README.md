# Metadata markers examples

Metadata markers are used to set descriptive information about processes and threads in trace output. This metadata helps organize and identify different parts of your trace when viewing it in trace visualization tools.

## Notes

* Those are _regular_ events, and it's optimal to use them after calling `CXXET_sink_thread_reserve()`
* Process metadata (name, sort index, labels) should typically be set once per process
  * Multiple process labels can be set to categorize or tag your process
  * Currently (2024-08) ignored by <https://ui.perfetto.dev>
* Thread metadata (name, sort index) should be set once per thread
* If e.g. name is provided more than once, <https://ui.perfetto.dev> uses the last provided value
  * Different tools may behave in other ways
* Sort indices control the display order in trace viewers
  * Currently (2024-08) doesn't work in <https://ui.perfetto.dev>

# Sink diversion examples

Generated events can be redirected to different output destinations (sinks) during runtime. This is useful for separating events from your application's execution into different files.

## Examples

* [1.cxx](1.cxx) demonstrates how to switch between collecting events into a local `file_sink` and the default global one on the same thread.
* [2.cxx](2.cxx) shows how events from different threads can be redirected to a local thread safe `file_sink`.
* [3.cxx](3.cxx) displays thread unsafe `file_sink` used across multiple threads (in a safe manner!).
* [4.cxx](4.cxx) presents how to create & use `cascade_sink` - both thread safe and unsafe version.
* TODO e.g. tracing of request processing within a server, where each request is independent and processed in its own thread

## Notes

* Always call `CXXET_sink_thread_flush()` before switching sinks to ensure all pending events are written out to the desired sink.
* Local sinks can be created as thread safe (`true`) or not (`false`).
* Use `divert_thread_sink_to_this()` to redirect current thread's events to the given instance of a `sink`.
* Use `CXXET_sink_thread_divert_to_sink_global()` to restore the default `sink` setup.

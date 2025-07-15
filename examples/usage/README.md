# Examples

This directory contains examples demonstrating the usage of the `cxxet` library for tracing `C++` code.

It's lightweight library that generates `chrome trace` format output, which can be viewed in Chrome's built-in trace viewer (`chrome://tracing`) or tools like [Perfetto UI](https://ui.perfetto.dev/).

## Categories

* [complete](complete/README.md)
  * Marker for measuring time spans, with begin and end merged into single event.
* [counter](counter/README.md)
  * Marker for tracking changing values over time.
* [duration](duration/README.md)
  * Markers for measuring time spans, with begin and end marked separately.
* [for_tests](for_tests/README.md)
  * Various examples showing how to (not) use provided library.
* [instant](instant/README.md)
  * Marker for point-in-time events.
* [sink_diversion](sink_diversion/README.md)
  * Advanced manipulation and creation of local & intermediate sinks for markers.

Examples are executed in acompanying `bats` test suite, while asserting on the output and desired behavior. See [`01_suite.bats`](../tests/integration/suite/01_suite.bats) for more details.

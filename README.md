# Runtime simple markers

TODO ...

Examples are [here](examples/README.md).

## Explorations

### Various formats - are they feasible, widely used, etc.?

* [using `Tracy` profiler to display saved traces](doc/UsingTracyProfiler.md)
* [`Chrome trace` format](doc/UsingChromeTrace.md)
* TODO explore formats described in <https://www.brendangregg.com/FlameGraphs/cpuflamegraphs.html>

### Competition?

* [`opentracing-cpp`](https://github.com/opentracing/opentracing-cpp)
  * Archived.
  * Seems hard to use.
* [`Perfetto Trace Processor`](https://perfetto.dev/docs/analysis/trace-processor)
  * Doesn't seem to provide API for tracing itself, only processing results.
  * Citation: "The Trace Processor is a C++ library (/src/trace_processor) that ingests traces encoded in a wide variety of formats ..." => TODO explore more or not?
* [`Datadog` - Tracing C++ Applications](https://docs.datadoghq.com/tracing/trace_collection/automatic_instrumentation/dd_libraries/cpp/?code-lang=cpp) - [github repo.](https://github.com/DataDog/dd-trace-cpp)
  * seems bound to greater `Datadog` ecosystem => probably not feasible
* [`OpenTelemetry C++`](https://opentelemetry.io/docs/languages/cpp/) - [github repo.](https://github.com/open-telemetry/opentelemetry-cpp)
  * looks large and complex => probably not feasible

## TODO

Maybe create github issues for those?

* better & wider test coverage
  * [recommendation](https://github.com/Ruzovej/compare_unit_test_frameworks/blob/main/docs/g++_release.md#doctest-with-macro-as-above--using-binary-asserts) for unit test usage
* benchmarking
  * try reimplementing the `rsm::marker` to don't hold all the data, but "save" them immediately and update end time on `submit`/d-ror (by holding only pointer to this particular record)
* add more output formats (e.g. implied by this conversation: <https://chatgpt.com/share/681b2833-c728-8003-9e28-a4e1c1c35196>)
  * read e.g. <https://www.chromium.org/developers/how-tos/trace-event-profiling-tool/> and fix how it's saved -> e.g. ~~"stacked" markers are properly displayed~~
* [TODO.md](doc/TODO.md)

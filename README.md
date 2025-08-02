# `cxxet` = C++ easy tracing

## License

![LGPLv3 License](doc/lgplv3-with-text-154x68.png)

## Introduction

TODO ...; see/mention/interlink (even more ...) presentation about [intrusive & visual profiling for Prague `C++` meetup](https://github.com/Ruzovej/visual_and_intrusive_profiling_presentation) here.

## Examples

[Examples are here](examples/README.md).

## Explorations

### (Various?) format(s) - are they feasible, widely used, etc.?

* [`Chrome trace` format](doc/UsingChromeTrace.md)
  * currently used, the only option so far and seems it will suffice
* [using `Tracy` profiler to display saved traces](doc/UsingTracyProfiler.md)
  * TODO would it be feasible to implement `Tracy's` internal format too?!
* TODO explore formats described in <https://www.brendangregg.com/FlameGraphs/cpuflamegraphs.html>
* TODO some nice [article](https://igortodorovskiibm.github.io/blog/2024/09/19/tracing/) 

### Competition?

* [`NVIDIA Nsight Systems`](https://developer.nvidia.com/nsight-systems)
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

* benchmarking
  * try reimplementing the `cxxet::marker` to don't hold all the data, but "save" them immediately and update end time on `submit`/d-ror (by holding only pointer to this particular record)
* add more output formats (e.g. implied by this conversation: <https://chatgpt.com/share/681b2833-c728-8003-9e28-a4e1c1c35196>)
  * read e.g. <https://www.chromium.org/developers/how-tos/trace-event-profiling-tool/> and fix how it's saved -> e.g. ~~"stacked" markers are properly displayed~~
* [TODO.md](doc/TODO.md)

# `cxxet` = C++ easy tracing

<!--
https://docs.github.com/en/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax#section-links
-->

- [License](#license)
- [Introduction](#introduction)
- [Examples](#examples)
- [Development and management in general](#development-and-management-in-general)
  - [Preparing & using `docker` environment](#preparing--using-docker-environment)
- [Explorations](#explorations)
- [TODO](#todo)

## License

[`LGPLv3`](https://www.gnu.org/licenses/lgpl-3.0.html) -> [COPYING](COPYING) & [COPYING.lesser](COPYING.LESSER)

![LGPLv3 image](doc/lgplv3-with-text-154x68.png)

Your application can be proprietary, commercial, or under any license. You can link `cxxet` dynamically or statically without releasing your application's source code, as long as You:

- include copy of LGPLv3 license with distribution,
- preserve `cxxet`'s copyright notices,
- allow users to replace `cxxet` (provide object files or ensure dynamic linking remains possible),
- if you modify `cxxet` itself, those modifications must be released under `LGPLv3`.

## Introduction

TODO ...; see/mention/interlink (even more ...) presentation about [intrusive & visual profiling for Prague `C++` meetup](https://github.com/Ruzovej/visual_and_intrusive_profiling_presentation) here.

## Examples

[Examples are here](examples/README.md).

## Development and management in general

When having `bash` available, use `cxxet_manage.bash` for compilation, testing, preparing `docker` image(s) for development, and so on. Good starting point is to display what is available, e.g.:

```bash
$ ./cxxet_manage.bash --help
...
$ ./cxxet_manage.bash compile --help
...
```

Further details about provided convenience scripts are in respective [`README.md`](scripts/README.md).

### Preparing & using `docker` environment

If you don't have all tools/compilers/... available and don't want to install them "directly" (see their list in respective `Dockerfile`s), corresponding `docker` image(s) can be built & used:

```bash
# `XYZ` is valid image name or `--help`/`-h`:
$ ./cxxet_manage.bash docker_build_image XYZ # builds given image (or displays help ...)
...
$ ./cxxet_manage.bash docker_interactive XYZ # enters interactive (bash) shell in it (or displays help ...)
...
# all further commands are executed inside via the `cxxet_manage.bash` script too, e.g.:
$ ./cxxet_manage.bash compile ...
...
```

## Explorations

### (Various?) format(s) - are they feasible, widely used, etc.?

- [`Chrome trace` format](doc/UsingChromeTrace.md)
  - currently used, the only option so far and seems it will suffice
- [using `Tracy` profiler to display saved traces](doc/UsingTracyProfiler.md)
  - TODO would it be feasible to implement `Tracy's` internal format too?!
- TODO explore formats described in <https://www.brendangregg.com/FlameGraphs/cpuflamegraphs.html>
- TODO some nice [article](https://igortodorovskiibm.github.io/blog/2024/09/19/tracing/) 

### Competition?

- [`NVIDIA Nsight Systems`](https://developer.nvidia.com/nsight-systems)
- [`opentracing-cpp`](https://github.com/opentracing/opentracing-cpp)
  - Archived.
  - Seems hard to use.
- [`Perfetto Trace Processor`](https://perfetto.dev/docs/analysis/trace-processor)
  - Doesn't seem to provide API for tracing itself, only processing results.
  - Citation: "The Trace Processor is a C++ library (/src/trace_processor) that ingests traces encoded in a wide variety of formats ..." => TODO explore more or not?
- [`Datadog` - Tracing C++ Applications](https://docs.datadoghq.com/tracing/trace_collection/automatic_instrumentation/dd_libraries/cpp/?code-lang=cpp) - [github repo.](https://github.com/DataDog/dd-trace-cpp)
  - seems bound to greater `Datadog` ecosystem => probably not feasible
- [`OpenTelemetry C++`](https://opentelemetry.io/docs/languages/cpp/) - [github repo.](https://github.com/open-telemetry/opentelemetry-cpp)
  - looks large and complex => probably not feasible

## TODO

Maybe create github issues for those?

- benchmarking
  - try reimplementing the `cxxet::marker` to don't hold all the data, but "save" them immediately and update end time on `submit`/d-ror (by holding only pointer to this particular record)
- add more output formats (e.g. implied by this conversation: <https://chatgpt.com/share/681b2833-c728-8003-9e28-a4e1c1c35196>)
  - read e.g. <https://www.chromium.org/developers/how-tos/trace-event-profiling-tool/> and fix how it's saved -> e.g. ~~"stacked" markers are properly displayed~~
- [TODO.md](doc/TODO.md)

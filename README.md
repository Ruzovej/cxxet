# Runtime simple markers

TODO ...

* [using `Tracy` profiler to display saved traces](doc/UsingTracyProfiler.md)

## TODO

* this file - description, usage, etc.
* `docker`ize it
  * support for `clang`/`gcc`
  * add `ccache`?!
  * add `clang-tidy`/`cppcheck`/`cpplint`/`clang-format`/... ?!
  * hieararchy of `Dockerfile` - "release" env. <- "dev" env. <- "test" env., etc.
    * on top of that, add `devcontainer` addon support
* refactor `CMakeLists.txt`, etc.
  * split into multiple files
* better & wider test coverage
  * [recommendation](https://github.com/Ruzovej/compare_unit_test_frameworks/blob/main/docs/g++_release.md#doctest-with-macro-as-above--using-binary-asserts) for unit test usage
* benchmarking
  * try reimplementing the `rsm::marker` to don't hold all the data, but "save" them immediately and update end time on `submit`/d-ror (by holding only pointer to this particular record)
* add more output formats (e.g. implied by this conversation: <https://chatgpt.com/share/681b2833-c728-8003-9e28-a4e1c1c35196>)
  * read e.g. <https://www.chromium.org/developers/how-tos/trace-event-profiling-tool/> and fix how it's saved -> e.g. ~~"stacked" markers are properly displayed~~
* consider simplifying the `thread` & `global` initialization?
* consider removing `impl::thread::active` & initialize it automatically?
* compare "Meyers singleton" vs. "global, file scope singletons"? Both `static` and `thread_local` ...
* fix `shared` version of library
* `windows` support
* CI/CD ...?
* [TODO.md](doc/TODO.md)
* get inspired by [`Tracy`'s features](https://github.com/wolfpld/tracy/releases/latest/download/tracy.pdf), e.g.:
  * 3.15 `Automated data collection`
    * .3 `Context switches`
    * .6 `Hardware sampling`
    * etc.
  * 6 `Exporting zone statistics to CSV`
  * 7 `Importing external profiling data`

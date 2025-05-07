# Runtime simple markers

TODO ...

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
* benchmarking
  * try reimplementing the `rsm::marker` to don't hold all the data, but "save" them immediately and update end time on `submit`/d-ror (by holding only pointer to this particular record)
* add more output formats (e.g. implied by this conversation: <https://chatgpt.com/share/681b2833-c728-8003-9e28-a4e1c1c35196>)
* consider simplifying the `thread` & `global` initialization?
* consider removing `impl::thread::active` & initialize it automatically?
* "Meyers singleton" vs. "global, file scope singletons"? Both `static` and `thread_local` ...
* fix `shared` version of library
* `windows` support
* CI/CD ...?

# Using [`Tracy`](https://github.com/wolfpld/tracy) profiler to display saved traces

## compiling needed stuff

```bash
$ time git clone git@github.com:wolfpld/tracy.git
...
# profiler GUI:
$ time cmake -B tracy/build -S tracy/profiler -DCMAKE_BUILD_TYPE=Release -DLEGACY=ON
...
$ time cmake --build tracy/build -j "$(nproc)"
...
# format converter:
$ time cmake -B tracy/build_import -S tracy/import -DCMAKE_BUILD_TYPE=Release
...
$ time cmake --build tracy/build_import -j "$(nproc)"
...
```

## example of converting generated `chrome trace` file

```bash
# generate trace file
$ rsm/bin/release/rsm_example_counter_2 /tmp/trace.json
...
# convert it to `tracy` format
$ tracy/build_import/tracy-import-chrome /tmp/trace.json /tmp/trace.tracy
...
# display it in `Tracy profiler` GUI
$ tracy/build/tracy-profiler /tmp/trace.tracy
...
```

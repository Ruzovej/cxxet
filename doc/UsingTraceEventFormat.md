# [`Trace Event Format`](https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU)

TODO:

1. cleanup in this document
    * maybe reorder the subparagraphs? Probably the most important (using `Perfetto` UI) is now last ...
2. explore formats described in <https://www.brendangregg.com/FlameGraphs/cpuflamegraphs.html>
3. [TODO](#todo) below ...

## Displaying it

### Conversion option

It can be displayed by `Tracy` profiler (see [Using Tracy profiler to display saved traces](UsingTracyProfiler.md)), but in reduced form.

### "Native" tools

There are several tools, list of what isn't deprecated & was tried follows:

#### `Chrome` browser

Open <chrome://tracing> and load desired file:

* Seems really simple, but at the same time it does its job.
* If doing this for the first time, it may require some setup, such as enabling this tool or similar.
* Navigation is a bit tricky, press `?` and learn key shortcuts.

#### [`Speedscope`](https://github.com/jlfwong/speedscope) -> <https://www.speedscope.app/>

Click on `Browse`, select desired file and proceed.

* Again, very simple, but seems to do the job.

#### [`Perfetto`](https://ui.perfetto.dev/)

Click on `Open trace file`, select desired file and proceed.

* Seems more modern, navigation (press press `?` for help) is IMHO worse than in `Chrome` or `Speedscope`.

## TODO

* Implement other kind of events (["`phases`"](https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU/edit?tab=t.0#heading=h.puwqg050lyuy)); at least those mentioned by [`tracy-import-chrome`](https://github.com/wolfpld/tracy/blob/master/import/src/import-chrome.cpp#L31):

```bash
$ ./tracy-import-chrome --help
Usage: import-chrome input.json output.tracy

The following chrome-tracing phases are supported:

  b/B/e/E - Timeline events such as ZoneNamed
  X - Timeline events such as ZoneNamed
  i/I - Message events such as TracyMessage
    * Messages containing the word "frame" are interpreted as frame events such as FrameMarkNamed
  C - Plot events such as TracyPlot
  M - Metadata of type "thread_name" is used to name threads
```

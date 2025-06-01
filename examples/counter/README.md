# Counter marker examples

Counter markers are used to track changing numerical values over time. They _may_ appear as line graphs in the trace viewer and are perfect for monitoring metrics like memory usage, CPU utilization, operation counts, or any other quantitative data during program execution.

## Performance Tips

* Use `RSM_thread_local_sink_reserve(capacity)` when you know how many events you'll generate.
* Counter markers have (low?) overhead that can accumulate in high-frequency loops.

## Notes

* Counter value type is `double`, which should have enough precision for most use cases.

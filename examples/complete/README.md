# Complete marker example

Complete marker is used to measure and visualize time span.

## Notes

* Represented by single event.
  * Requires roughly half the space of `Duration`-pair markers.
* Less error prone than manual `Duration` markers - almost impossible to misuse thanks to its RAII-style implementation.

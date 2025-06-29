# Duration markers examples

Duration markers are used to measure and visualize time spans. RAII-style and manual begin/end markers are provided.

## Notes

* Begin and end of any such given trace are saved as two separate events.
  * Requires roughly 2 times more space than `Complete` marker.
* More flexible.
  * This "flexibility" hardly manifests in current implementation, when markers are dumped all at once.
* When manually specifying begin and corresponding end, it's less safe than `complete` markers or its `RAII` version.

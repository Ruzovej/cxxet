# Categories example

During trace processing, visualization and/or analysis, categories allow filtering trace events into logical groups.

## Notes

* Each category gets a unique bit flag that can be combined using bitwise OR operations.
* During registration, categories can be assigned custom bit positions (e.g., `1024` == `1u << 10`).
* Events can belong to multiple categories simultaneously.
* Events without explicit categories aren't categorized.
* Unknown category flags are silently ignored.

# Categories examples

During trace processing, visualization and/or analysis, categories allow filtering trace events into logical groups.

## Examples

* [1.cxx](1.cxx) demonstrates basic category registration and event categorization.
* [2.cxx](2.cxx) shows how registration fails when incorrect name is provided.
* [3.cxx](3.cxx) shows how registration fails when incorrect explicit flag is provided.
* [4.cxx](4.cxx) shows how registration succeeds (when correctly specified) and fails when duplicate flag is provided.

## Notes

* Each category gets a unique bit flag that can be combined using bitwise OR operations.
* During registration, categories can be assigned custom bit positions (e.g., `1024` == `1u << 10`).
* Events can belong to multiple categories simultaneously.
* Events without explicit categories aren't categorized.
* Unknown category flags are silently ignored.

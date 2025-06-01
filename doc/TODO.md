# TODO explore those

## [PROBABLY RESOLVED] `tsan` error

seems to be same problem as this one: <https://stackoverflow.com/q/73136532> ... accepted answer states that this may be caused by combining `ubsan` & `tsan` ... -> ~~for now, considering it **flaky & "external" bug**. Fix: do not use those two sanitizers together, but **ignoring it** for now~~ keeping the `tsan` "alone".

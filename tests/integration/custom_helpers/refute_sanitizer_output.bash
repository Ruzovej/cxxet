#!/usr/bin/env bash

function refute_sanitizer_output() {
    refute_output --partial "runtime error: " # `ubsan` seems to generate messages such as this one
    refute_output --partial "ThreadSanitizer"
    refute_output --partial "LeakSanitizer"
    refute_output --partial "AddressSanitizer"
}

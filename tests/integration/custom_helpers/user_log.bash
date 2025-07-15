#!/usr/bin/env bash

function user_log() {
    local fmt_string="$1"
    shift
    printf "${fmt_string}" "$@" >&3
}

add_executable(
    infra_sanitizer_check
        san_check.cxx
)
add_sanitizers(infra_sanitizer_check)
add_unix_compiler_flags(infra_sanitizer_check)

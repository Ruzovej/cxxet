add_executable(
    infra_sanitizer_check
        san_check.cpp
)
add_sanitizers(infra_sanitizer_check)

add_executable(
    infra_sanitizer_check
        san_check.cpp
)

if (RSM_WITH_ASAN)
    add_sanitizer(infra_sanitizer_check address)
    add_sanitizer(infra_sanitizer_check leak)
    add_sanitizer(infra_sanitizer_check undefined)
endif()
if (RSM_WITH_TSAN)
    add_sanitizer(infra_sanitizer_check thread)
endif()

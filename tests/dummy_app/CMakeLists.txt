add_executable(
    rsm_dummy_app
        "main.cpp"
)
target_link_libraries(
    rsm_dummy_app
        PRIVATE
        rsm
)

add_executable(
    rsm_infra_sanitizer_check
        san_check.cpp
)
if (RSM_WITH_ASAN)
    add_sanitizer(rsm_infra_sanitizer_check address)
    add_sanitizer(rsm_infra_sanitizer_check leak)
    add_sanitizer(rsm_infra_sanitizer_check undefined)
endif()
if (RSM_WITH_TSAN)
    add_sanitizer(rsm_infra_sanitizer_check thread)
    #add_sanitizer(rsm_infra_sanitizer_check undefined)
endif()

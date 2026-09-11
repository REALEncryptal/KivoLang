cmake_minimum_required(VERSION 3.20)

project(kivo LANGUAGES C)

file(GLOB_RECURSE KIVO_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

function(kivo_configure_target target)
    set_target_properties(${target} PROPERTIES
        C_STANDARD 17
        C_STANDARD_REQUIRED YES
        C_EXTENSIONS NO
    )

    target_include_directories(${target} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
    )

    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

add_executable(kivo ${KIVO_SOURCES})
kivo_configure_target(kivo)

# Tests reuse the language code, but provide their own main().
set(KIVO_MODULE_SOURCES ${KIVO_SOURCES})
list(REMOVE_ITEM KIVO_MODULE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.c")

include(CTest)
if(BUILD_TESTING)
    file(GLOB_RECURSE KIVO_TEST_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_*.c"
    )

    foreach(test_source IN LISTS KIVO_TEST_SOURCES)
        # Use the relative path so different test folders may reuse a filename.
        file(RELATIVE_PATH test_path "${CMAKE_CURRENT_SOURCE_DIR}/tests" "${test_source}")
        string(REGEX REPLACE "\\.c$" "" test_name "${test_path}")
        string(REPLACE "/" "__" test_target "${test_name}")
        set(test_target "kivo_test_${test_target}")

        add_executable(${test_target} "${test_source}" ${KIVO_MODULE_SOURCES})
        kivo_configure_target(${test_target})
        add_test(NAME "${test_name}" COMMAND ${test_target})
        set_tests_properties("${test_name}" PROPERTIES
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        )
    endforeach()
endif()

cmake_minimum_required(VERSION 3.20)

project(kivo LANGUAGES C)

add_executable(kivo
    src/main.c
)

set_target_properties(kivo PROPERTIES
    C_STANDARD 17
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO
)

target_include_directories(kivo PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(MSVC)
    target_compile_options(kivo PRIVATE /W4)
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(kivo PRIVATE -Wall -Wextra -Wpedantic)
endif()

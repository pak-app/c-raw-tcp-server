cmake_minimum_required(VERSION 3.10)

project(c_raw_tcp_server C)

set(CMAKE_C_STANDARD 99)

# Add include directory for headers
include_directories(include)

# List source files
set(SOURCES
    src/main.c
    src/tcp.c
)

add_executable(server ${SOURCES})

# ✅ Add compiler warnings (shadowing, etc.)
if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(server PRIVATE -Wall -Wextra -Wshadow -Wstrict-prototypes)
endif()

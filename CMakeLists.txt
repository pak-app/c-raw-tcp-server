cmake_minimum_required(VERSION 3.10)

project(c_http_web_server C)

set(CMAKE_C_STANDARD 99)

# Add include directory for headers
include_directories(include)

# List source files
set(SOURCES
    src/main.c
    src/tool.c
)

add_executable(c_http_web_server ${SOURCES})

cmake_minimum_required(VERSION 3.20)

project(Concrete VERSION 0.1.0 LANGUAGES CXX C)

set (CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (CMAKE_HOST_APPLE)
    add_compile_definitions(APPLE=1)
else ()
    add_compile_definitions(APPLE=0)
endif ()

if (CMAKE_HOST_LINUX)
    add_compile_definitions(LINUX=1)
else ()
    add_compile_definitions(LINUX=0)
endif ()

if (CMAKE_HOST_WIN32)
    add_compile_definitions(WINDOWS=1)
else ()
    add_compile_definitions(WINDOWS=0)
endif ()

add_subdirectory(Source)

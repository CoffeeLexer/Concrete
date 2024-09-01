cmake_minimum_required(VERSION 3.20)

project(Concrete VERSION 0.1.0 LANGUAGES CXX C)

set (CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(Source)

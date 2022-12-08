cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 14)      
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(PROJ_MAIN_NAME "queue_benchmarks")
project(${PROJ_MAIN_NAME})
find_package(Klepsydra REQUIRED)

add_subdirectory(benchmarks)
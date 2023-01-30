# Copyright 2023 Klepsydra Technologies AG
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set(PROJECT_NAME queue_benchmarks)
project(${PROJECT_NAME})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(benchmark REQUIRED)

FILE(GLOB_RECURSE ${PROJECT_NAME}_HEADERS
    ${CMAKE_SOURCE_DIR}/modules/include/*.h
)
FILE(GLOB_RECURSE ${PROJECT_NAME}_SRC
    ${CMAKE_SOURCE_DIR}/modules/src/*.cpp
)

#SAFE QUEUE
add_executable(${PROJECT_NAME}_safe_queue src/safe_queue_benchmarks.cpp)
target_link_libraries(${PROJECT_NAME}_safe_queue kpsr_core benchmark::benchmark_main)

#DEQUEUE
add_executable(${PROJECT_NAME}_dequeue ${${PROJECT_NAME}_HEADERS} ${${PROJECT_NAME}_SRC} src/dequeue_benchmarks.cpp)

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME}_dequeue
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../modules/include 
  )

target_link_libraries(${PROJECT_NAME}_dequeue benchmark::benchmark_main)

#CONCURRENT QUEUE
add_executable(${PROJECT_NAME}_concurrent_queue src/concurrent_queue_benchmarks.cpp)
target_link_libraries(${PROJECT_NAME}_concurrent_queue kpsr_core benchmark::benchmark_main)
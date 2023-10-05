// Copyright 2023 Klepsydra Technologies AG
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <benchmark/benchmark.h>
#include <chrono>
#include <concurrentqueue.h>

moodycamel::ConcurrentQueue<std::string> cQueue;

void ConcurrentQueueParameters(benchmark::internal::Benchmark* benchmark) {
    benchmark->ArgNames({"QSize"});
    benchmark->Args({256});
}

static void ConcurrentQueuePush(benchmark::State &state)
{
    
    size_t size = state.range(0);
    
    for (auto _ : state) {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < size; i++) {
            cQueue.enqueue(std::to_string(i));

        }
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end -
                                                                                         start);

        state.SetIterationTime(elapsed_seconds.count());

        for (size_t i = 0; i < size; i++) {
            std::string item = std::to_string(i);
            cQueue.try_dequeue(item);
        }
    }
}

static void ConcurrentQueuePop(benchmark::State &state)
{
    size_t size = state.range(0);

    for (auto _ : state) {
        for (size_t i = 0; i < size; i++) {
            cQueue.enqueue(std::to_string(i));
        }
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < size; i++) {
            std::string item = std::to_string(i);
            cQueue.try_dequeue(item);
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end -
                                                                                         start);

        state.SetIterationTime(elapsed_seconds.count());
    }
}

BENCHMARK(ConcurrentQueuePush)->Apply(ConcurrentQueueParameters)->DenseThreadRange(10, 50, 10)->UseRealTime();
BENCHMARK(ConcurrentQueuePop)->Apply(ConcurrentQueueParameters)->DenseThreadRange(10, 50, 10)->UseRealTime();

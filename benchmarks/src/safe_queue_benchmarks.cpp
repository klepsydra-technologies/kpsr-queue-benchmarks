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
#include <klepsydra/mem_core/safe_queue.h>

#include <iostream>

void QueueParameters(benchmark::internal::Benchmark* benchmark) {
    benchmark->ArgNames({"QSize"});

    for (int e = 8; e <= 256; e*=2) {
        benchmark->Args({e});
    }
}

std::unique_ptr<kpsr::mem::SafeQueue<std::string>> safeQueue;

void DoSetup(const benchmark::State &state)
{
    safeQueue = std::make_unique<kpsr::mem::SafeQueue<std::string>>(state.range(0));
}

static void SafeQueuePush(benchmark::State &state)
{
    size_t nThreads = state.threads();
    size_t itemByThread = state.range(0)/nThreads;
    size_t startIndex = itemByThread * state.thread_index(); 
    size_t endIndex = startIndex + itemByThread;

    for (auto _ : state) {

        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = startIndex; i < endIndex; i++) {
            safeQueue->push(std::to_string(i));
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end -
                                                                                         start);

        state.SetIterationTime(elapsed_seconds.count());
        for (size_t i = startIndex; i < endIndex; i++) {
            std::string item = std::to_string(i); 
            safeQueue->pop(item);
        }
    }
}

static void SafeQueuePop(benchmark::State &state)
{
    size_t nThreads = state.threads();
    size_t itemByThread = state.range(0)/nThreads;
    size_t startIndex = itemByThread * state.thread_index(); 
    size_t endIndex = startIndex + itemByThread;

    for (auto _ : state) {
        for (size_t i = startIndex; i < endIndex; i++) {
            safeQueue->push(std::to_string(i));
        }

        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = startIndex; i < endIndex; i++) {
            std::string item = std::to_string(i); 
            safeQueue->pop(item);
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end -
                                                                                         start);

        state.SetIterationTime(elapsed_seconds.count());
    }
}

BENCHMARK(SafeQueuePush)->Apply(QueueParameters)->ThreadRange(1, 8)->UseRealTime()->Setup(DoSetup);
BENCHMARK(SafeQueuePop)->Apply(QueueParameters)->ThreadRange(1, 8)->UseRealTime()->Setup(DoSetup);
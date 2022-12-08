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
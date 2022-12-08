#include <benchmark/benchmark.h>
#include <chrono>
#include <concurrentqueue.h>

moodycamel::ConcurrentQueue<std::string> cQueue;

void ConcurrentQueueParameters(benchmark::internal::Benchmark* benchmark) {
    benchmark->ArgNames({"QSize"});

    for (int e = 2; e <= 64; e*=2) {
        benchmark->Args({e});
    }
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

BENCHMARK(ConcurrentQueuePush)->Apply(ConcurrentQueueParameters)->ThreadRange(1,8)->UseRealTime();
BENCHMARK(ConcurrentQueuePop)->Apply(ConcurrentQueueParameters)->ThreadRange(1,8)->UseRealTime();
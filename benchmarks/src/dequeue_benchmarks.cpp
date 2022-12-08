#include <benchmark/benchmark.h>
#include <chrono>
#include <thread>
#include <deque.h>
#include <iostream>

void DequeueParameters(benchmark::internal::Benchmark* benchmark) {
    benchmark->ArgNames({"QSize"});

    for (int e = 8; e <= 256; e*=2) {
        benchmark->Args({e});
    }
}

std::shared_ptr<deque::Worker<std::string>> worker;
std::shared_ptr<deque::Stealer<std::string>> stealer;

void DoGeneralSetup(const benchmark::State &state)
{
    size_t size = state.range(0);
    auto ws = deque::deque<std::string>();
    worker = std::make_shared<deque::Worker<std::string>>(std::move(ws.first));
}

void DoMultithreadedSetup(const benchmark::State &state)
{
    size_t size = state.range(0);
    auto ws = deque::deque<std::string>();
    worker = std::make_shared<deque::Worker<std::string>>(std::move(ws.first));
    stealer = std::make_shared<deque::Stealer<std::string>>(std::move(ws.second));
    for (size_t i = 0; i < size; i++) {
        worker->push(std::to_string(i));
    }
}


static void DequeuePush(benchmark::State &state)
{
    size_t size = state.range(0);

    for (auto _ : state) {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < size; i++) {
            worker->push(std::to_string(i));
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end -
                                                                                         start);

        state.SetIterationTime(elapsed_seconds.count());
        for (size_t i = 0; i < size; i++) {
            worker->pop();
        }
    }
}

static void DequeuePop(benchmark::State &state)
{
    size_t size = state.range(0);

    for (auto _ : state) {
        for (size_t i = 0; i < size; i++) {
            worker->push(std::to_string(i));
        }
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < size; i++) {
            worker->pop();
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end -
                                                                                         start);

        state.SetIterationTime(elapsed_seconds.count());
    }
}

static void DequeuePopMultiThreaded(benchmark::State &state)
{
    size_t size = state.range(0);
    auto stealear_copy = stealer;

    for (auto _ : state) {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < size; i++) {
            stealear_copy->steal();
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end -
                                                                                         start);

        state.SetIterationTime(elapsed_seconds.count());
    }
}

BENCHMARK(DequeuePush)->Apply(DequeueParameters)->UseRealTime()->Setup(DoGeneralSetup);
BENCHMARK(DequeuePop)->Apply(DequeueParameters)->UseRealTime()->Setup(DoGeneralSetup);
BENCHMARK(DequeuePopMultiThreaded)->Apply(DequeueParameters)->ThreadRange(1, 8)->UseRealTime()->Setup(DoMultithreadedSetup);
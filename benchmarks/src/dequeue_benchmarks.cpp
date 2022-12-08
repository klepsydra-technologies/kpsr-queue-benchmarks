#include <benchmark/benchmark.h>
#include <chrono>
#include <thread>
#include <deque.h>
#include <iostream>

void DequeueParameters(benchmark::internal::Benchmark* benchmark) {
    benchmark->ArgNames({"QSize"});

    for (int e = 2; e <= 64; e*=2) {
        benchmark->Args({e});
    }
}

static void DequeuePush(benchmark::State &state)
{
    size_t size = state.range(0);

    for (auto _ : state) {
        auto ws = deque::deque<std::string>();
        auto worker = std::move(ws.first);
        auto stealer = std::move(ws.second);
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < size; i++) {
            worker.push(std::to_string(i));
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end -
                                                                                         start);

        state.SetIterationTime(elapsed_seconds.count());
    }
}

static void DequeuePop(benchmark::State &state)
{
    size_t size = state.range(0);

    for (auto _ : state) {
        auto ws = deque::deque<std::string>();
        auto worker = std::move(ws.first);
        auto stealer = std::move(ws.second);
        for (size_t i = 0; i < size; i++) {
            worker.push(std::to_string(i));
        }
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < size; i++) {
            worker.pop();
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end -
                                                                                         start);

        state.SetIterationTime(elapsed_seconds.count());
    }
}

static void DequeuePushAndPop(benchmark::State &state)
{
    size_t size = state.range(0);

    for (auto _ : state) {
        auto ws = deque::deque<std::string>();
        auto worker = std::move(ws.first);
        auto stealer = std::move(ws.second);
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < size; i++) {
            worker.push(std::to_string(i));
        }
        for (size_t i = 0; i < size; i++) {
            worker.pop();
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end -
                                                                                         start);

        state.SetIterationTime(elapsed_seconds.count());
    }
}

static void DequeuePushAndPopTwoThreads(benchmark::State &state)
{
    size_t qSize = state.range(0);
    size_t bmNumberOfPushes = qSize * 10;

    for (auto _ : state) {
        auto ws = deque::deque<std::string>();
        auto worker = std::move(ws.first);
        auto stealer = std::move(ws.second);
        bool pushThreadFinished = false;
        bool popThreadFinished = false;
        std::atomic<int> actualNumberOfPushes(0);
        int actualNumberOfPops = 0;

        auto start = std::chrono::high_resolution_clock::now();
        std::thread threadPush([&bmNumberOfPushes, &pushThreadFinished, &worker, &actualNumberOfPushes]() {
            for (size_t i = 0; i < bmNumberOfPushes; i++) {
                worker.push(std::to_string(i));
                actualNumberOfPushes++;
            }
            pushThreadFinished = true;
        });
        std::thread threadPop([&bmNumberOfPushes, &popThreadFinished, &stealer, &actualNumberOfPushes, &actualNumberOfPops]() {
            auto stealear_copy = stealer;
            while (actualNumberOfPops < bmNumberOfPushes) {
                if (actualNumberOfPops < actualNumberOfPushes) {
                    stealear_copy.steal();
                    actualNumberOfPops++;
                }
            }
            popThreadFinished = true;
        });
        while(!pushThreadFinished || !popThreadFinished) {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        };

        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end -
                                                                                         start);

        state.SetIterationTime(elapsed_seconds.count());
        threadPush.join();
        threadPop.join();
    }
}

static void DequeuePushAndPopFourThreads(benchmark::State &state)
{
    size_t qSize = state.range(0);
    size_t bmNumberOfPushes = qSize * 10;

    for (auto _ : state) {
        auto ws = deque::deque<std::string>();
        auto worker = std::move(ws.first);
        auto stealer = std::move(ws.second);
        bool pushThreadFinished = false;
        bool popThreadFinished1 = false;
        bool popThreadFinished2 = false;
        bool popThreadFinished3 = false;
        std::atomic<int> actualNumberOfPushes(0);
        std::atomic<int> actualNumberOfPops(0);

        auto start = std::chrono::high_resolution_clock::now();
        std::thread threadPush([&bmNumberOfPushes, &pushThreadFinished, &worker, &actualNumberOfPushes]() {
            for (size_t i = 0; i < bmNumberOfPushes; i++) {
                worker.push(std::to_string(i));
                actualNumberOfPushes++;
            }
            pushThreadFinished = true;
        });

        std::thread threadPop1([&bmNumberOfPushes, &popThreadFinished1, &stealer, &actualNumberOfPushes, &actualNumberOfPops]() {
            auto stealear_copy = stealer;
            while (actualNumberOfPops < bmNumberOfPushes) {
                if (actualNumberOfPops < actualNumberOfPushes) {
                    stealear_copy.steal();
                    actualNumberOfPops++;
                }
            }
            popThreadFinished1 = true;
        });

        std::thread threadPop2([&bmNumberOfPushes, &popThreadFinished2, &stealer, &actualNumberOfPushes, &actualNumberOfPops]() {
            auto stealear_copy = stealer;
            while (actualNumberOfPops < bmNumberOfPushes) {
                if (actualNumberOfPops < actualNumberOfPushes) {
                    stealear_copy.steal();
                    actualNumberOfPops++;
                }
            }
            popThreadFinished2 = true;
        });

        std::thread threadPop3([&bmNumberOfPushes, &popThreadFinished3, &stealer, &actualNumberOfPushes, &actualNumberOfPops]() {
            auto stealear_copy = stealer;
            while (actualNumberOfPops < bmNumberOfPushes) {
                if (actualNumberOfPops < actualNumberOfPushes) {
                    stealear_copy.steal();
                    actualNumberOfPops++;
                }
            }
            popThreadFinished3 = true;
        });

        while(!pushThreadFinished || !popThreadFinished1 || !popThreadFinished2 || !popThreadFinished3) {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        };

        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end -
                                                                                         start);

        state.SetIterationTime(elapsed_seconds.count());
        threadPush.join();
        threadPop1.join();
        threadPop2.join();
        threadPop3.join();
    }
}

BENCHMARK(DequeuePush)->Apply(DequeueParameters)->UseRealTime();
BENCHMARK(DequeuePop)->Apply(DequeueParameters)->UseRealTime();
BENCHMARK(DequeuePushAndPop)->Apply(DequeueParameters)->UseRealTime();
BENCHMARK(DequeuePushAndPopTwoThreads)->Apply(DequeueParameters)->UseRealTime();
BENCHMARK(DequeuePushAndPopFourThreads)->Apply(DequeueParameters)->UseRealTime();
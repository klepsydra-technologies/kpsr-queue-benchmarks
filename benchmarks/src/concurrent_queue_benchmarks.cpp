#include <benchmark/benchmark.h>
#include <chrono>
#include <concurrentqueue.h>

void ConcurrentQueueParameters(benchmark::internal::Benchmark* benchmark) {
    benchmark->ArgNames({"QSize"});

    for (int e = 2; e <= 64; e*=2) {
        benchmark->Args({e});
    }
}

static void ConcurrentQueuePush(benchmark::State &state)
{
    
    size_t size = state.range(0);
    moodycamel::ConcurrentQueue<std::string> cQueue;
    
    for (auto _ : state) {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < size; i++) {
            cQueue.enqueue(std::to_string(i));
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end -
                                                                                         start);

        state.SetIterationTime(elapsed_seconds.count());
    }
}

static void ConcurrentQueuePop(benchmark::State &state)
{
    size_t size = state.range(0);
    moodycamel::ConcurrentQueue<std::string> cQueue;

    for (auto _ : state) {
        for (size_t i = 0; i < size; i++) {
            cQueue.enqueue(std::to_string(i));
        }
        std::string popValue;
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < size; i++) {
            cQueue.try_dequeue(popValue);
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end -
                                                                                         start);

        state.SetIterationTime(elapsed_seconds.count());
    }
}

static void ConcurrentQueuePushAndPop(benchmark::State &state)
{
    size_t size = state.range(0);

    for (auto _ : state) {
        moodycamel::ConcurrentQueue<std::string> cQueue;
        std::string popValue;
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < size; i++) {
            cQueue.enqueue(std::to_string(i));
        }
        for (size_t i = 0; i < size; i++) {
            cQueue.try_dequeue(popValue);
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end -
                                                                                         start);

        state.SetIterationTime(elapsed_seconds.count());
    }
}

static void ConcurrentQueuePushAndPopTwoThreads(benchmark::State &state)
{
    size_t qSize = state.range(0);
    size_t bmNumberOfPushes = qSize * 10;

    for (auto _ : state) {
        moodycamel::ConcurrentQueue<std::string> cQueue;
        bool pushThreadFinished = false;
        bool popThreadFinished = false;
        std::atomic<int> actualNumberOfPushes(0);
        int actualNumberOfPops = 0;

        auto start = std::chrono::high_resolution_clock::now();
        std::thread threadPush([&bmNumberOfPushes, &pushThreadFinished, &cQueue, &actualNumberOfPushes]() {
            for (size_t i = 0; i < bmNumberOfPushes; i++) {
                if (cQueue.enqueue(std::to_string(i))) {
                    actualNumberOfPushes++;
                }
            }
            pushThreadFinished = true;
        });
        std::thread threadPop([&bmNumberOfPushes, &popThreadFinished, &cQueue, &actualNumberOfPushes, &actualNumberOfPops]() {
            std::string popValue;
            while (actualNumberOfPops < bmNumberOfPushes) {
                if (actualNumberOfPops < actualNumberOfPushes && cQueue.try_dequeue(popValue)) {
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

static void ConcurrentQueuePushAndPopFourThreads(benchmark::State &state)
{
    size_t qSize = state.range(0);
    size_t bmNumberOfPushes = qSize * 10;

    for (auto _ : state) {
        moodycamel::ConcurrentQueue<std::string> cQueue;    
        bool pushThreadFinished = false;
        bool popThreadFinished1 = false;
        bool popThreadFinished2 = false;
        bool popThreadFinished3 = false;
        std::atomic<int> actualNumberOfPushes(0);
        std::atomic<int> actualNumberOfPops(0);

        auto start = std::chrono::high_resolution_clock::now();
        std::thread threadPush([&bmNumberOfPushes, &pushThreadFinished, &cQueue, &actualNumberOfPushes]() {
            for (size_t i = 0; i < bmNumberOfPushes; i++) {
                if (cQueue.enqueue(std::to_string(i))) {
                    actualNumberOfPushes++;
                }
            }
            pushThreadFinished = true;
        });

        std::thread threadPop1([&bmNumberOfPushes, &popThreadFinished1, &cQueue, &actualNumberOfPushes, &actualNumberOfPops]() {
            std::string popValue;
            while (actualNumberOfPops < bmNumberOfPushes) {
                if (actualNumberOfPops < actualNumberOfPushes && cQueue.try_dequeue(popValue)) {
                    actualNumberOfPops++;
                }
            }
            popThreadFinished1 = true;
        });

        std::thread threadPop2([&bmNumberOfPushes, &popThreadFinished2, &cQueue, &actualNumberOfPushes, &actualNumberOfPops]() {
            std::string popValue;
            while (actualNumberOfPops < bmNumberOfPushes) {
                if (actualNumberOfPops < actualNumberOfPushes && cQueue.try_dequeue(popValue)) {
                    actualNumberOfPops++;
                }
            }
            popThreadFinished2 = true;
        });

        std::thread threadPop3([&bmNumberOfPushes, &popThreadFinished3, &cQueue, &actualNumberOfPushes, &actualNumberOfPops]() {
            std::string popValue;
            while (actualNumberOfPops < bmNumberOfPushes) {
                if (actualNumberOfPops < actualNumberOfPushes && cQueue.try_dequeue(popValue)) {
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

BENCHMARK(ConcurrentQueuePush)->Apply(ConcurrentQueueParameters)->UseRealTime();
BENCHMARK(ConcurrentQueuePop)->Apply(ConcurrentQueueParameters)->UseRealTime();
BENCHMARK(ConcurrentQueuePushAndPop)->Apply(ConcurrentQueueParameters)->UseRealTime();
BENCHMARK(ConcurrentQueuePushAndPopTwoThreads)->Apply(ConcurrentQueueParameters)->UseRealTime();
BENCHMARK(ConcurrentQueuePushAndPopFourThreads)->Apply(ConcurrentQueueParameters)->UseRealTime();
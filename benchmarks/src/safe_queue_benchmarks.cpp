#include <benchmark/benchmark.h>
#include <chrono>
#include <klepsydra/mem_core/safe_queue.h>


void QueueParameters(benchmark::internal::Benchmark* benchmark) {
    benchmark->ArgNames({"QSize"});

    for (int e = 2; e <= 64; e*=2) {
        benchmark->Args({e});
    }
}

static void SafeQueuePush(benchmark::State &state)
{
    size_t size = state.range(0);

    for (auto _ : state) {
        kpsr::mem::SafeQueue<std::string> safeQueue(size);
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < size; i++) {
            safeQueue.push(std::to_string(i));
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end -
                                                                                         start);

        state.SetIterationTime(elapsed_seconds.count());
    }
}

static void SafeQueuePop(benchmark::State &state)
{
    size_t size = state.range(0);

    for (auto _ : state) {
        kpsr::mem::SafeQueue<std::string> safeQueue(size);
        for (size_t i = 0; i < size; i++) {
            safeQueue.push(std::to_string(i));
        }
        std::string popValue;
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < size; i++) {
            safeQueue.pop(popValue);
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end -
                                                                                         start);

        state.SetIterationTime(elapsed_seconds.count());
    }
}

static void SafeQueuePushAndPop(benchmark::State &state)
{
    size_t size = state.range(0);

    for (auto _ : state) {
        kpsr::mem::SafeQueue<std::string> safeQueue(size);
        std::string popValue;
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < size; i++) {
            safeQueue.push(std::to_string(i));
        }
        for (size_t i = 0; i < size; i++) {
            safeQueue.pop(popValue);
        }
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end -
                                                                                         start);

        state.SetIterationTime(elapsed_seconds.count());
    }
}

static void SafeQueuePushAndPopTwoThreads(benchmark::State &state)
{
    size_t qSize = state.range(0);
    size_t bmNumberOfPushes = qSize * 10;

    for (auto _ : state) {
        kpsr::mem::SafeQueue<std::string> safeQueue(qSize);
        bool pushThreadFinished = false;
        bool popThreadFinished = false;
        std::atomic<int> actualNumberOfPushes(0);
        int actualNumberOfPops = 0;

        auto start = std::chrono::high_resolution_clock::now();
        std::thread threadPush([&bmNumberOfPushes, &pushThreadFinished, &safeQueue, &actualNumberOfPushes]() {
            for (size_t i = 0; i < bmNumberOfPushes; i++) {
                if (safeQueue.push(std::to_string(i))) {
                    actualNumberOfPushes++;
                }
            }
            pushThreadFinished = true;
        });
        std::thread threadPop([&bmNumberOfPushes, &popThreadFinished, &safeQueue, &actualNumberOfPushes, &actualNumberOfPops]() {
            std::string popValue;
            while (actualNumberOfPops < bmNumberOfPushes) {
                if (actualNumberOfPops < actualNumberOfPushes && safeQueue.try_pop(popValue)) {
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

static void SafeQueuePushAndPopFourThreads(benchmark::State &state)
{
    size_t qSize = state.range(0);
    size_t bmNumberOfPushes = qSize * 10;

    for (auto _ : state) {
        kpsr::mem::SafeQueue<std::string> safeQueue(qSize);
        bool pushThreadFinished = false;
        bool popThreadFinished1 = false;
        bool popThreadFinished2 = false;
        bool popThreadFinished3 = false;
        std::atomic<int> actualNumberOfPushes(0);
        std::atomic<int> actualNumberOfPops(0);

        auto start = std::chrono::high_resolution_clock::now();
        std::thread threadPush([&bmNumberOfPushes, &pushThreadFinished, &safeQueue, &actualNumberOfPushes]() {
            for (size_t i = 0; i < bmNumberOfPushes; i++) {
                if (safeQueue.push(std::to_string(i))) {
                    actualNumberOfPushes++;
                }
            }
            pushThreadFinished = true;
        });
        std::thread threadPop1([&bmNumberOfPushes, &popThreadFinished1, &safeQueue, &actualNumberOfPushes, &actualNumberOfPops]() {
            std::string popValue;
            while (actualNumberOfPops < bmNumberOfPushes) {
                if (actualNumberOfPops < actualNumberOfPushes && safeQueue.try_pop(popValue)) {
                    actualNumberOfPops++;
                }
            }
            popThreadFinished1 = true;
        });

        std::thread threadPop2([&bmNumberOfPushes, &popThreadFinished2, &safeQueue, &actualNumberOfPushes, &actualNumberOfPops]() {
            std::string popValue;
            while (actualNumberOfPops < bmNumberOfPushes) {
                if (actualNumberOfPops < actualNumberOfPushes && safeQueue.try_pop(popValue)) {
                    actualNumberOfPops++;
                }
            }
            popThreadFinished2 = true;
        });

        std::thread threadPop3([&bmNumberOfPushes, &popThreadFinished3, &safeQueue, &actualNumberOfPushes, &actualNumberOfPops]() {
            std::string popValue;
            while (actualNumberOfPops < bmNumberOfPushes) {
                if (actualNumberOfPops < actualNumberOfPushes && safeQueue.try_pop(popValue)) {
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

BENCHMARK(SafeQueuePush)->Apply(QueueParameters)->UseRealTime();
BENCHMARK(SafeQueuePop)->Apply(QueueParameters)->UseRealTime();
BENCHMARK(SafeQueuePushAndPop)->Apply(QueueParameters)->UseRealTime();
BENCHMARK(SafeQueuePushAndPopTwoThreads)->Apply(QueueParameters)->UseRealTime();
BENCHMARK(SafeQueuePushAndPopFourThreads)->Apply(QueueParameters)->UseRealTime();
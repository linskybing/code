#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>

constexpr int THREADS = 4;
constexpr int ITERATIONS = 10'000'000;
constexpr int CACHE_LINE_SIZE = 64;

struct SharedData {
    std::atomic<int> value;
};

struct PaddedData {
    std::atomic<int> value;
    char padding[CACHE_LINE_SIZE - sizeof(std::atomic<int>)];
};

template <typename T>
void benchmark(T data[], const std::string& description) {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i = 0; i < THREADS; i++) {
        threads.emplace_back([&, i]() {
            for (int j = 0; j < ITERATIONS; j++) {
                data[i].value.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    std::cout << description << " Execution time: " << elapsed.count() << " s\n";
}

int main() {
    SharedData sharedData[THREADS]; 
    PaddedData paddedData[THREADS]; 

    std::cout << "False Sharing Test:\n";
    benchmark(sharedData, "False Sharing");

    std::cout << "\nAvoid False Sharing Test:\n";
    benchmark(paddedData, "Avoid False Sharing");

    return 0;
}

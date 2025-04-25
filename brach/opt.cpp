#include <iostream>
#include <chrono>

int main() {
    const int N = 100000000;  // Set the number of iterations
    int sum = 0;

    auto start = std::chrono::high_resolution_clock::now();  // Record start time
    for (int i = 0; i < N; ++i) {
        int add = (i % 2 == 0) ? i : -i;  // Direct calculation, avoiding if-else
        sum += add;
    }
    auto end = std::chrono::high_resolution_clock::now();  // Record end time

    std::chrono::duration<double> duration = end - start;
    std::cout << "Total sum: " << sum << "\n";
    std::cout << "Time taken without branch: " << duration.count() << " seconds\n";

    return 0;
}

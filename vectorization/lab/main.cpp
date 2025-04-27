#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <immintrin.h>
#define TILE 32
#define min(a, b) ((a < b)? a : b)

using real_t = float;

void write_matrix(const std::string &filename, const real_t *data, size_t count, bool append=false) {
    std::ofstream ofs;
    if (append)
        ofs.open(filename, std::ios::binary | std::ios::app);
    else
        ofs.open(filename, std::ios::binary);

    if (!ofs) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        std::exit(1);
    }
    ofs.write(reinterpret_cast<const char*>(data), count * sizeof(real_t));
}


inline void matmul_avx2(real_t *A, real_t *B, real_t *C, int M, int K, int N) {
    for (int i = 0; i < M; ++i) {
        for (int k = 0; k < K; ++k) {
            __m256 a_vec = _mm256_set1_ps(A[i * K + k]);
            for (int j = 0; j < N - 7; j += 8) {
                __m256 b_vec = _mm256_load_ps(&B[k * N + j]);
                __m256 c_vec = _mm256_load_ps(&C[i * N + j]);
                _mm256_store_ps(&C[i * N + j], _mm256_add_ps(_mm256_mul_ps(a_vec, b_vec), c_vec));
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <M> <K> <N>" << std::endl;
        return 1;
    }

    int M = std::stoi(argv[1]);
    int K = std::stoi(argv[2]);
    int N = std::stoi(argv[3]);

    // Aligned allocations for better vectorization
    real_t *A = reinterpret_cast<real_t*>(aligned_alloc(32, M * K * sizeof(real_t)));
    real_t *B = reinterpret_cast<real_t*>(aligned_alloc(32, K * N * sizeof(real_t)));
    real_t *C = reinterpret_cast<real_t*>(aligned_alloc(32, M * N * sizeof(real_t)));

    memset(C, 0, M * N * sizeof(real_t));

    if (!A || !B || !C) {
        std::cerr << "Aligned allocation failed" << std::endl;
        return 1;
    }

    // Read data from files: A then B from input.bin
    std::ifstream ifs("input.bin", std::ios::binary);
    if (!ifs) {
        std::cerr << "Cannot open input.bin" << std::endl;
        return 1;
    }
    ifs.read(reinterpret_cast<char*>(A), static_cast<size_t>(M) * K * sizeof(real_t));
    ifs.read(reinterpret_cast<char*>(B), static_cast<size_t>(K) * N * sizeof(real_t));
    ifs.close();  // Close the file after reading

    // Matrix multiplication
    auto start = std::chrono::high_resolution_clock::now();
    
    matmul_avx2(A, B, C, M, K, N);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Execution time: " << elapsed.count() << " s\n";

    write_matrix("c.bin", C, static_cast<size_t>(M) * N, false);

    // Free allocated memory
    free(A);
    free(B);
    free(C);

    return 0;
}

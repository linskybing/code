#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <immintrin.h>
#define TILE 64
#define min(a, b) ((a < b)? a : b)

using real_t = float;

inline real_t* aligned_malloc(size_t n) {
    return static_cast<real_t*>(_mm_malloc(n * sizeof(real_t), 32));
}

inline void aligned_free(real_t* ptr) {
    _mm_free(ptr);
}


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

inline void matmul_avx2(real_t *A, real_t *B, real_t *C, int block_i, int block_j, int block_k, const int M, const int K, const int N) {
    int i_end = min(block_i + TILE, M);
    int k_end = min(block_k + TILE, K);
    int j_end = min(block_j + TILE, N);

    const int VLEN = 8;
    // [TODO] convert to avx2 version
    for (int i = block_i; i < i_end; ++i) {
        for (int k = block_k; k < k_end; ++k) {

            __m256 a_vec = _mm256_set1_ps(A[i * K + k]);

            int j = block_j;
            for (; j <= j_end - VLEN; j += VLEN) {
                __m256 c_vec = _mm256_loadu_ps(&C[i * N + j]);
                __m256 b_vec = _mm256_loadu_ps(&B[k * N + j]);

                c_vec = _mm256_fmadd_ps(a_vec, b_vec, c_vec);

                _mm256_storeu_ps(&C[i * N + j], c_vec);
            }

            for (; j < j_end; ++j) {
                C[i*N + j] += A[i*K + k] * B[k*N + j];
            }
        }
    }
}


void matmul_tile(real_t *A, real_t *B, real_t *C, int M, int K, int N) {
    memset(C, 0, M * N * sizeof(real_t));

    for (int block_j = 0; block_j < N; block_j += TILE)
        for (int block_i = 0; block_i < M; block_i += TILE)
            for (int block_k = 0; block_k < K; block_k += TILE)
                matmul_avx2(A, B, C, block_i, block_j, block_k, M, K, N);
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
    real_t *A = aligned_malloc(M * K);
    real_t *B = aligned_malloc(K * N);
    real_t *C = aligned_malloc(M * N);

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
    
    matmul_tile(A, B, C, M, K, N);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Execution time: " << elapsed.count() << " s\n";

    write_matrix("c.bin", C, static_cast<size_t>(M) * N, false);

    // Free allocated memory
    aligned_free(A);
    aligned_free(B);
    aligned_free(C);

    return 0;
}

#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <chrono>
#include <iostream>
#define SIZE 2048

typedef float real_t;

// Horizontal sum of 8 elements in AVX2 vector
inline real_t hsum_avx(__m256 v) {
    // Extract high 128 bits and add to low 128 bits
    __m128 vlow = _mm256_castps256_ps128(v);
    __m128 vhigh = _mm256_extractf128_ps(v, 1);
    vlow = _mm_add_ps(vlow, vhigh);
    
    // Continue horizontal addition in SSE
    __m128 shuf = _mm_movehdup_ps(vlow);  // Broadcast elements 3,1 to 2,0
    __m128 sums = _mm_add_ps(vlow, shuf);
    shuf = _mm_movehl_ps(shuf, sums);     // Move high to low
    sums = _mm_add_ss(sums, shuf);        // Final sum in low 32 bits
    return _mm_cvtss_f32(sums);
}

void matmul_avx2(real_t *A, real_t *B, real_t *C, int m, int k, int n) {
    const int VLEN = 8;  // AVX2 processes 8 real_ts per vector
    memset(C, 0, m * n * sizeof(real_t));
    
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            __m256 acc = _mm256_setzero_ps();  // Accumulator for dot product
            
            // Process elements in vectorized chunks
            for (int l = 0; l < k; l += VLEN) {
                // Load 8 elements from A's row and B's column (transposed)
                __m256 a = _mm256_loadu_ps(&A[i * k + l]);
                __m256 b = _mm256_loadu_ps(&B[j * k + l]);
                
                // Fused multiply-add operation: acc += a * b
                acc = _mm256_fmadd_ps(a, b, acc);
            }
            
            // Horizontal sum of accumulator and store result
            C[i * n + j] = hsum_avx(acc);
        }
    }
}

int main() {
    const int M = SIZE, K = SIZE, N = SIZE;
    bool flag = false;
    
    // Aligned allocations for better vectorization
    real_t *A = (real_t*) aligned_alloc(32, M * K * sizeof(real_t));   // 32-byte aligned for AVX2
    real_t *B_ = (real_t*) aligned_alloc(32, K * N * sizeof(real_t));
    real_t *B = (real_t*) aligned_alloc(32, K * N * sizeof(real_t));
    real_t *C = (real_t*) aligned_alloc(32, M * N * sizeof(real_t));

    // Initialize matrices
    std::fill(A, A + M*K, 1.0f);
    std::fill(B_, B_ + K*N, 2.0f);

    auto start = std::chrono::high_resolution_clock::now();
    
    // Transpose B_ to B (column-major to row-major)
    for (int i = 0; i < K; ++i) {
        for (int j = 0; j < N; ++j) {
            B[j * K + i] = B_[i * N + j];  // Corrected indexing for transpose
        }
    }

    matmul_avx2(A, B, C, M, K, N);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Execution time: " << elapsed.count() << " s\n";

    // Verify results (expected value: 4096 * 2 = 8192)
    for (int i = 0; i < M * N; ++i) {
        if ((C[i] != (SIZE * 2))) {
            flag = true;
            std::cout << "Mismatch at " << i << ": " << C[i] << std::endl;
            break;
        }
    }
    
    std::cout << (flag ? "Wrong result!" : "Correct result") << std::endl;
    
    free(A); free(B_); free(B); free(C);
    return 0;
}
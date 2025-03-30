#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <chrono>
#include <iostream>
#define SIZE 4096


void matmul_avx512(float *A, float *B, float *C, int m, int k, int n) {
    const int VLEN = 16;
    memset(C, 0, m * n * sizeof(float));
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            __m512 c = _mm512_setzero_ps(); // 用来累加 C(i,j) 的值

            for (int l = 0; l < k; l += VLEN) {
                // 加载 A 的一行（1x16）
                __m512 a = _mm512_loadu_ps(&A[i * k + l]);
                
                // 加载 B 的一列（16x1，已转置）
                __m512 b = _mm512_loadu_ps(&B[j * k + l]);

                // 计算 A 的当前元素与 B 的当前列的乘积并累加
                c = _mm512_fmadd_ps(a, b, c);
            }

            // 将结果存入 C(i,j)
            _mm512_storeu_ps(&C[i * n + j], c);
        }
    }
}

int main() {
    const int M = SIZE, K = SIZE, N = SIZE;
    bool flag = false;
    float *A = (float*) aligned_alloc(64, M * K * sizeof(float));
    float *B_ = (float*) aligned_alloc(64, K * N * sizeof(float));
    float *B = (float*) aligned_alloc(64, K * N * sizeof(float));
    float *C = (float*) aligned_alloc(64, M * N * sizeof(float));

    for (int i = 0; i < M * K; ++i) A[i] = 1.0f;
    for (int i = 0; i < K * N; ++i) B_[i] = 2.0f;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < K; ++i) {
        for (int j = 0; j < N; ++j) {
            B[j * M + i] = B_[i * N + j];
        }
    }

    matmul_avx512(A, B, C, M, K, N);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Execution time: " << elapsed.count() << " s\n";

    for (int i = 0; i < M * N; ++i) {
        if (C[i] != (2.0f) * SIZE) {
            flag = true;
            printf("%f\n", C[i]);
            break;
        }
    }
    printf(((flag)? "wrong answer!!\n": "correct answer\n"));
    free(A); free(B); free(C);
    return 0;
}
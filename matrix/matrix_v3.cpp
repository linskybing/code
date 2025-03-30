#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <iostream>
#define SIZE 1024
#define TILE 32
#define ROUND (SIZE / TILE)

void matmul(float *A, float *B, float *C, int M, int K, int N) {
    memset(C, 0, M * N * sizeof(float));
    for (int block_i = 0; block_i < ROUND; ++block_i) {
        for (int block_k = 0; block_k < ROUND; ++block_k) {
            for (int block_j = 0; block_j < ROUND; ++block_j) {
                for (int i = block_i * TILE; i < (block_i + 1) * TILE; ++i) {
                    for (int k = block_k * TILE; k < (block_k + 1) * TILE; ++k) {
                        for (int j = block_j * TILE; j < (block_j + 1) * TILE; ++j) {
                            C[i * N + j] += A[i * K + k] * B[k * N + j];
                        }
                    }
                }
            }
        }
    }
}

int main() {
    const int M = SIZE, K = SIZE, N = SIZE;
    bool flag = false;
    float *A = (float*) aligned_alloc(64, M * K * sizeof(float));
    float *B = (float*) aligned_alloc(64, K * N * sizeof(float));
    float *C = (float*) aligned_alloc(64, M * N * sizeof(float));

    for (int i = 0; i < M * K; ++i) A[i] = 1.0f;
    for (int i = 0; i < K * N; ++i) B[i] = 2.0f;

    auto start = std::chrono::high_resolution_clock::now();
    matmul(A, B, C, M, K, N);
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
#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#define SIZE 4096
void matmul_avx512(float *A, float *B, float *C, int M, int K, int N) {
    const int VLEN = 16;
    memset(C, 0, M * N * sizeof(float));
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < M; ++i) {
        for (int k = 0; k < K; ++k) {
            __m512 a_vec = _mm512_set1_ps(A[i * K + k]);
            for (int j = 0; j < N; j += VLEN) {
                __m512 b_vec = _mm512_loadu_ps(&B[k * N + j]);
                __m512 c_vec = _mm512_loadu_ps(&C[i * N + j]);
                c_vec = _mm512_fmadd_ps(a_vec, b_vec, c_vec);
                _mm512_storeu_ps(&C[i * N + j], c_vec);
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

    matmul_avx512(A, B, C, M, K, N);

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
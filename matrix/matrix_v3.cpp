#include <stdio.h>
#include <string.h>
#include <omp.h>
#define N 4096
#define TILE 64

float a[N][N], b[N][N], c[N][N];

inline void mat_mul_block(int si, int sj, int sk) {
    for (int i = si; i < si + TILE; ++i) {
        #pragma GCC unroll 8
        for (int k = sk; k < sk + TILE; ++k) {
            #pragma GCC unroll 64
            for (int j = sj; j < sj + TILE; ++j) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

void fgemm() {
    #pragma omp parallel for collapse(2)
    for (int si = 0; si < N; si += TILE)
        for (int sj = 0; sj < N; sj += TILE) {
            for (int sk = 0; sk < N; sk += TILE)
                mat_mul_block(si, sj, sk);
        }

}

int main() {
    FILE *fa = fopen("a.dat", "rb"), *fb = fopen("b.dat", "rb"), *fc = fopen("c.dat", "wb");
    fread(a, sizeof(float), sizeof a / sizeof(float), fa);
    fread(b, sizeof(float), sizeof b / sizeof(float), fb);
    fclose(fa);
    fclose(fb);

    memset(c, 0, sizeof c);
    fgemm();

    fwrite(c, sizeof(float), sizeof c / sizeof(float), fc);
    fclose(fc);

    return 0;
}
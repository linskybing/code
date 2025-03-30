#include <stdio.h>
#include <string.h>

#define N 4096

float a[N][N], b[N][N], c[N][N];

void mat_mul() {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < N; i++)
        for (int k = 0; k < N; k++) {
            #pragma GCC unroll 64
            for (int j = 0; j < N; j++)
                c[i][j] += a[i][k] * b[k][j];
        }
}

int main() {
    FILE *fa = fopen("a.dat", "rb"), *fb = fopen("b.dat", "rb"), *fc = fopen("c.dat", "wb");
    fread(a, sizeof(float), sizeof a / sizeof(float), fa);
    fread(b, sizeof(float), sizeof b / sizeof(float), fb);
    fclose(fa);
    fclose(fb);

    memset(c, 0, sizeof c);
    mat_mul();

    fwrite(c, sizeof(float), sizeof c / sizeof(float), fc);
    fclose(fc);

    return 0;
}
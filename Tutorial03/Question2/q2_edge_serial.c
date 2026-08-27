/* Q2: edge detection on an n x n image with a 3x3 template -- serial version.
 * Plain C, no libraries beyond stdio/stdlib/time.
 * usage: ./q2_edge_serial <n> [out.pgm]
 * prints CSV: n,time */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* 3x3 Laplacian template: exactly 9 multiply-add operations per pixel, which
 * is the cost model given in the question, T_s = 9 * t_c * n^2. */
static const int TEMPLATE[3][3] = {
    { -1, -1, -1 },
    { -1,  8, -1 },
    { -1, -1, -1 }
};

static double now(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + 1e-9 * ts.tv_nsec;
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: %s <n> [out.pgm]\n", argv[0]); return 1; }
    int n = atoi(argv[1]);
    if (n < 3) { fprintf(stderr, "n must be >= 3\n"); return 1; }

    unsigned char *in  = malloc((size_t)n * n);
    unsigned char *out = calloc((size_t)n * n, 1);
    if (!in || !out) { perror("malloc"); return 1; }

    /* Deterministic synthetic image: vertical ramp, checker blocks and a disc.
     * No randomness, so the serial and parallel runs see identical pixels. */
    int blk = n / 8 + 1;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            int v = (i * 255) / n;
            if (((i / blk) + (j / blk)) % 2) v = 255 - v;
            if ((i - n/2)*(i - n/2) + (j - n/2)*(j - n/2) < (n/4)*(n/4)) v = 200;
            in[(size_t)i * n + j] = (unsigned char)v;
        }

    double t0 = now();
    for (int i = 1; i < n - 1; i++)                  /* border pixels stay 0 */
        for (int j = 1; j < n - 1; j++) {
            int acc = 0;
            for (int di = -1; di <= 1; di++)
                for (int dj = -1; dj <= 1; dj++)
                    acc += TEMPLATE[di+1][dj+1] * in[(size_t)(i+di) * n + (j+dj)];
            if (acc < 0) acc = -acc;
            out[(size_t)i * n + j] = (unsigned char)(acc > 255 ? 255 : acc);
        }
    double t = now() - t0;

    printf("%d,%.6f\n", n, t);

    if (argc > 2) {
        FILE *f = fopen(argv[2], "wb");
        if (f) {
            fprintf(f, "P5\n%d %d\n255\n", n, n);
            fwrite(out, 1, (size_t)n * n, f);
            fclose(f);
        } else perror(argv[2]);
    }
    free(in); free(out);
    return 0;
}

/* Q2: edge detection on an n x n image with a 3x3 template -- Pthreads version.
 * Pure pthreads: create, join, nothing else. No mutex, no condition variable,
 * no barrier, no atomics -- the threads are simply turned loose.
 *
 * Why that is safe: every thread READS the same input image (read-only sharing
 * needs no protection) and WRITES a block of output rows that no other thread
 * touches. The 3x3 template of a thread's first and last row reaches into its
 * neighbours rows, but only in the INPUT buffer, which nobody modifies, so a
 * thread can never observe a half-written value. main reads the output only
 * after pthread_join, which is itself the synchronisation point.
 *
 * usage: ./q2_edge_pthreads <n> <nthreads> [out.pgm]
 * prints CSV: threads,n,time */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const int TEMPLATE[3][3] = {          /* 9 multiply-adds per pixel */
    { -1, -1, -1 },
    { -1,  8, -1 },
    { -1, -1, -1 }
};

static double now(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + 1e-9 * ts.tv_nsec;
}

typedef struct {
    const unsigned char *in;   /* shared, read-only */
    unsigned char *out;        /* shared, but this thread writes rows [r0,r1) only */
    int n, r0, r1;
} task_t;

static void *worker(void *arg) {
    task_t *t = (task_t *)arg;
    const unsigned char *in = t->in;
    unsigned char *out = t->out;
    int n = t->n;
    for (int i = t->r0; i < t->r1; i++)
        for (int j = 1; j < n - 1; j++) {
            int acc = 0;
            for (int di = -1; di <= 1; di++)
                for (int dj = -1; dj <= 1; dj++)
                    acc += TEMPLATE[di+1][dj+1] * in[(size_t)(i+di) * n + (j+dj)];
            if (acc < 0) acc = -acc;
            out[(size_t)i * n + j] = (unsigned char)(acc > 255 ? 255 : acc);
        }
    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 3) { fprintf(stderr, "usage: %s <n> <nthreads> [out.pgm]\n", argv[0]); return 1; }
    int n = atoi(argv[1]), p = atoi(argv[2]);
    if (n < 3 || p < 1) { fprintf(stderr, "need n >= 3 and nthreads >= 1\n"); return 1; }

    unsigned char *in  = malloc((size_t)n * n);
    unsigned char *out = calloc((size_t)n * n, 1);
    pthread_t *tid  = malloc(p * sizeof *tid);
    task_t   *task  = malloc(p * sizeof *task);
    if (!in || !out || !tid || !task) { perror("malloc"); return 1; }

    /* Same deterministic image as the serial program. */
    int blk = n / 8 + 1;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            int v = (i * 255) / n;
            if (((i / blk) + (j / blk)) % 2) v = 255 - v;
            if ((i - n/2)*(i - n/2) + (j - n/2)*(j - n/2) < (n/4)*(n/4)) v = 200;
            in[(size_t)i * n + j] = (unsigned char)v;
        }

    int rows = n - 2;                        /* interior rows 1 .. n-2 */
    double t0 = now();                       /* thread creation counts as overhead */
    for (int i = 0; i < p; i++) {
        task[i].in  = in;
        task[i].out = out;
        task[i].n   = n;
        task[i].r0  = 1 + (int)((long)rows * i / p);        /* even split, remainder */
        task[i].r1  = 1 + (int)((long)rows * (i + 1) / p);  /* spread over threads */
        if (pthread_create(&tid[i], NULL, worker, &task[i]) != 0) { perror("pthread_create"); return 1; }
    }
    for (int i = 0; i < p; i++) pthread_join(tid[i], NULL);
    double t = now() - t0;

    printf("%d,%d,%.6f\n", p, n, t);

    if (argc > 3) {
        FILE *f = fopen(argv[3], "wb");
        if (f) {
            fprintf(f, "P5\n%d %d\n255\n", n, n);
            fwrite(out, 1, (size_t)n * n, f);
            fclose(f);
        } else perror(argv[3]);
    }
    free(in); free(out); free(tid); free(task);
    return 0;
}

/* Q1: Monte Carlo estimation of pi -- Pthreads version.
 * Pure pthreads: pthread_create and pthread_join, nothing else. No mutex, no
 * condition variable, no barrier, no atomics.
 *
 * Why no lock is needed: the threads are completely independent. Each one has
 * its own generator seed and its own hit counter, both on its own stack, so
 * nothing shared is written during the loop. Each thread stores its count into
 * its own slot of task[] once, at the end, and main adds the counts up only
 * after pthread_join -- join is itself the synchronisation point (everything
 * the thread did happens-before the join returns). The order the counts are
 * added in does not matter: independent integers, associative addition.
 *
 * Why rand_r and not srand/rand: srand/rand keep one generator state in a
 * global, which glibc protects with an internal lock, so every thread would
 * queue up on the same generator instead of running freely. Measured on this
 * machine at N = 1e8: shared rand() took 3.36 s with 1 thread and 20.04 s with
 * 16 -- six times SLOWER, not faster. rand_r is the same generator with its
 * state passed in explicitly, one per thread, so the threads never touch each
 * other. Seeding each thread's `seed` is what srand() does, per stream.
 *
 * usage: ./q1_pi_pthreads <npoints> <nthreads>
 * prints CSV: threads,points,pi,error,time */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PI 3.14159265358979323846

static double now(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + 1e-9 * ts.tv_nsec;
}

typedef struct {
    long long n;              /* points this thread must generate */
    long long inside;         /* its own count, written once at the end */
    unsigned int seed;        /* its own generator stream */
} task_t;

static void *worker(void *arg) {
    task_t *t = (task_t *)arg;
    /* Everything the loop touches is on this thread's own stack. Keeping the
     * seed in the shared task[] array instead would make the loop write to
     * memory a neighbouring thread shares a cache line with (false sharing) --
     * that alone made 2 threads slower than 1 in an earlier version. */
    unsigned int seed = t->seed;
    long long n = t->n, inside = 0;
    for (long long i = 0; i < n; i++) {
        double x = rand_r(&seed) / (double)RAND_MAX;
        double y = rand_r(&seed) / (double)RAND_MAX;
        if (x * x + y * y <= 1.0) inside++;
    }
    t->inside = inside;
    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 3) { fprintf(stderr, "usage: %s <npoints> <nthreads>\n", argv[0]); return 1; }
    long long n = atoll(argv[1]);
    int p = atoi(argv[2]);
    if (n < 1 || p < 1) { fprintf(stderr, "need npoints >= 1 and nthreads >= 1\n"); return 1; }

    pthread_t *tid = malloc(p * sizeof *tid);
    task_t *task = malloc(p * sizeof *task);
    if (!tid || !task) { perror("malloc"); return 1; }

    double t0 = now();                        /* thread creation is parallel overhead */
    for (int i = 0; i < p; i++) {
        /* Even split; the remainder goes one point each to the first threads.
         * If p > n the surplus threads simply get zero points. */
        task[i].n = n / p + (i < n % p ? 1 : 0);
        task[i].inside = 0;
        task[i].seed = 1 + (unsigned int)i;   /* a distinct stream per thread */
        if (pthread_create(&tid[i], NULL, worker, &task[i]) != 0) { perror("pthread_create"); return 1; }
    }
    long long inside = 0;
    for (int i = 0; i < p; i++) {
        pthread_join(tid[i], NULL);
        inside += task[i].inside;             /* safe: the join already happened */
    }
    double t = now() - t0;

    double pi = 4.0 * (double)inside / (double)n;
    double err = pi - PI;
    if (err < 0) err = -err;
    printf("%d,%lld,%.10f,%.10f,%.6f\n", p, n, pi, err, t);
    free(tid); free(task);
    return 0;
}

/* Q1: Monte Carlo estimation of pi -- serial version.
 * Plain C: stdio/stdlib/time, nothing else.
 *
 * Random numbers come from the standard C generator. It is used through
 * rand_r(&seed) rather than srand()/rand() so that this program and the
 * Pthreads one use the exact same generator, which is what makes the speedup
 * numbers a fair comparison. rand_r is just rand with its state passed in
 * explicitly instead of hidden in a global; seeding `seed` below is what
 * srand() does.
 *
 * usage: ./q1_pi_serial <npoints>
 * prints CSV: points,pi,error,time */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PI 3.14159265358979323846

static double now(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + 1e-9 * ts.tv_nsec;
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: %s <npoints>\n", argv[0]); return 1; }
    long long n = atoll(argv[1]);
    if (n < 1) { fprintf(stderr, "npoints must be >= 1\n"); return 1; }

    unsigned int seed = 1;                       /* same as srand(1) */

    double t0 = now();
    long long inside = 0;
    for (long long i = 0; i < n; i++) {
        double x = rand_r(&seed) / (double)RAND_MAX;
        double y = rand_r(&seed) / (double)RAND_MAX;
        if (x * x + y * y <= 1.0) inside++;      /* point inside the unit circle */
    }
    double t = now() - t0;

    double pi = 4.0 * (double)inside / (double)n;
    double err = pi - PI;
    if (err < 0) err = -err;
    printf("%lld,%.10f,%.10f,%.6f\n", n, pi, err, t);
    return 0;
}

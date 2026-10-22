#include <stdio.h>
#include <stdlib.h> 
#include <omp.h>

int main() {
    const long long total_points = 1000000000LL;
    long long inside_count = 0;
    const int num_threads = 28;

    printf("Starting Monte Carlo Pi estimation with %d threads...\n", num_threads);
    double start_time = omp_get_wtime();

    #pragma omp parallel num_threads(num_threads) reduction(+:inside_count)
    {
        // Thread-private seed. Must be unsigned int for rand_r
        unsigned int seed = (unsigned int)omp_get_thread_num() + 1;

        #pragma omp for schedule(static)
        for (long long i = 0; i < total_points; i++) {
            // rand_r takes the address of the seed
            double x = (double)rand_r(&seed) / RAND_MAX;
            double y = (double)rand_r(&seed) / RAND_MAX;

            if (x * x + y * y <= 1.0) {
                inside_count++;
            }
        }
    }

    double end_time = omp_get_wtime();
    double pi_estimate = 4.0 * (double)inside_count / (double)total_points;

    printf("Estimated Pi: %.10f\n", pi_estimate);
    printf("Time: %.6f seconds\n", end_time - start_time);

    return 0;
}
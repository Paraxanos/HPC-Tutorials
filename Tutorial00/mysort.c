
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define DATA_SIZE 18250
#define MIN_TEMP -50.0
#define MAX_TEMP 50.0

// Generates random temperature data
void generate_data(double *data, int size) {
    for (int i = 0; i < size; i++) {
        data[i] = MIN_TEMP + ((double)rand() / RAND_MAX) * (MAX_TEMP - MIN_TEMP);
    }
}

// Verifies if the array is correctly sorted
int is_sorted(const double *data, int size) {
    for (int i = 0; i < size - 1; i++) {
        if (data[i] > data[i+1]) return 0;
    }
    return 1;
}

// 1. Bubble Sort
void bubble_sort(double *data, int size) {
    for (int i = 0; i < size - 1; i++) {
        int swapped = 0;
        for (int j = 0; j < size - i - 1; j++) {
            if (data[j] > data[j+1]) {
                double temp = data[j];
                data[j] = data[j+1];
                data[j+1] = temp;
                swapped = 1;
            }
        }
        // Optimization: stop early if array is already sorted
        if (!swapped) break;
    }
}

// 2. Quick Sort (Optimized - O(N log N))
void swap(double* a, double* b) {
    double t = *a;
    *a = *b;
    *b = t;
}

int partition(double *data, int low, int high) {
    double pivot = data[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (data[j] < pivot) {
            i++;
            swap(&data[i], &data[j]);
        }
    }
    swap(&data[i + 1], &data[high]);
    return (i + 1);
}

void quick_sort_recursive(double *data, int low, int high) {
    if (low < high) {
        int pi = partition(data, low, high);
        quick_sort_recursive(data, low, pi - 1);
        quick_sort_recursive(data, pi + 1, high);
    }
}

void quick_sort(double *data, int size) {
    quick_sort_recursive(data, 0, size - 1);
}

int main() {
    // added choice for cutom seed or random seed to make sure testing various seeds is easier, and to make reporoducibility easier and standardised.
    printf("Enter the seed for random number generation: press 0 to enter custom seed, or any other number to use purely random seed: ");
    int choice=0;
    scanf("%d", &choice);
    switch(choice){
        case 0:
            printf("Enter the custom seed: ");
            int custom_seed;
            scanf("%d", &custom_seed);
            srand(custom_seed);
            break;
        default:
            srand(time(0));
    }
    
    // Allocate memory for original dataset and a working copy
    double *original_data = (double *)malloc(DATA_SIZE * sizeof(double));
    double *working_copy = (double *)malloc(DATA_SIZE * sizeof(double));
    //I guess its a safe check for NULL pointers? but if neccesarry, can use perror() to catch errno flag/error codes.
    if (!original_data || !working_copy) {
        fprintf(stderr, "Memory allocation failed!\n");
        return 1;
    }
    
    // Generate the weather dataset
    generate_data(original_data, DATA_SIZE);
    
    struct timespec start, end;
    double time_spent;
    
    printf("Dataset Size: %d records (approx. 50 years of daily data)\n\n", DATA_SIZE);

    // --- Test 1: Bubble Sort ---
    // Copy original data to working copy to ensure identical starting conditions
    memcpy(working_copy, original_data, DATA_SIZE * sizeof(double));
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    bubble_sort(working_copy, DATA_SIZE);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("[Bubble Sort] Time: %f seconds | Verified: %s\n", 
           time_spent, is_sorted(working_copy, DATA_SIZE) ? "Yes" : "No");

    // --- Test 2: Quick Sort ---
    // Reset working copy with original unsorted data
    memcpy(working_copy, original_data, DATA_SIZE * sizeof(double));
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    quick_sort(working_copy, DATA_SIZE);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("[Quick Sort]  Time: %f seconds | Verified: %s\n", 
           time_spent, is_sorted(working_copy, DATA_SIZE) ? "Yes" : "No");

    // Cleanup
    free(original_data);
    free(working_copy);
    
    return 0;
}
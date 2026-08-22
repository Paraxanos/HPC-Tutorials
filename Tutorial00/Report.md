Answers to the questions proposed in the Tutorial

question 1 : Which sorting algorithm performed better? Explain your observation.

Answer : Quick Sort vastly outperformed Bubble Sort. In fact, Quick Sort executed so quickly that the gprof sampling profiler could barely register its execution time, rounding it to 0.00% across all three reports (-O0, -O2, -O3). Meanwhile, Bubble Sort consistently consumed 100.00% of the profiled CPU time (taking ~0.68 to ~0.82 seconds per run).

question 2 : Which function consumed the maximum execution time?

Answer : Across all three compilation phases, the ```bubble_sort``` function consumed the absolute maximum execution time, consistently registering 100.00% of the profiled self seconds (0.68s in -O0, 0.82s in -O2, and 0.81s in -O3).

question 3 : How does execution time change with increasing input size?

Answer : Projecting the runtime based on pure asymptotic complexities, under the condition that the input follows a uniform probability distribution, the bubble sort algorithm grows with an order of size O(N^2). However, quick sort grows by an order of O(N X log(N)). This indicates that as the problem size scales, the divergence between execution times of bubble and quick sort diverges by an order of N/log(N). So at truly large scale of problems (Like sorting daily temperatures from multiple regions for 50 years), the runtime gap is so significant that it completely voids the purpose of using bubble sort as an algorithm.

question 4 : How did compiler optimizations (-O2 and -O3) improve performance?

Answer : 
-O0 (Baseline / No Optimization):

The swap function is explicitly listed in the Call Graph.
The compiler performed a literal translation of C to Assembly. Every time partition needed to swap two elements, it executed a CALL instruction to jump to the swap function in memory, pushed arguments to the stack, and returned. In -O0, swap was called 155,422 times.

-O2 (Inlining):

GCC applied Function Inlining. Recognizing that swap is a tiny, frequently called function, the compiler copied the body of swap directly into the partition loop. This eliminated 155,422 function call/return overhead cycles, drastically reducing instruction count and improving the Instruction Per Cycle (IPC) metric.

-O3 (Vectorization)
-O3 enables aggressive loop transformations, dead-code elimination, and Auto-Vectorization (SIMD).
The compiler likely utilized AVX/SSE registers to compare and move multiple array elements simultaneously in a single CPU cycle.
partition was likely inlined directly into quick_sort_recursive, which was then unrolled or optimized into highly efficient machine code.

Note on Execution Time: While gprof shows similar total times (~0.8s) across runs (due to sampling variance and system background noise), a perf stat analysis would reveal that -O3 executes significantly fewer total CPU instructions and achieves a much higher IPC than -O0.

question 5 : Which algorithm would you recommend for large datasets? Justify your answer.

Answer : Quick sort over bubble sort. Not only is the baseline bubble sort slower than quick sort by orders of magnitude, but the quick sort algorithm is efficient in truly leveraging the optimizations offered by O2 flag and O3 flags, such as inlining and vectorisation (SIMD).Hence as problem size scales sharply, bubble sort provides the worst classical baseline for sorting, while quick sort provides one of the best (Some algorithms like radix sort may perform better.)
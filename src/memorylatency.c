#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define ITERATIONS 100000000

int default_test_sizes[36] = { 2, 4, 8, 12, 16, 24, 32, 48, 64, 96, 128, 192, 256, 512, 600, 768, 1024, 1536, 2048,
                               3072, 4096, 5120, 6144, 8192, 10240, 12288, 16384, 24567, 32768, 65536, 98304,
                               131072, 262144, 393216, 524288, 1048576 };

float RunTest(uint32_t size_kb, uint64_t iterations);

int main(int argc, char* argv[]) {
    (void) argv;

    int mobile_wait  = 0;
    if (argc > 1) {
        mobile_wait = 1;
    }

    printf("Region,Latency (ns)\n");
    for (long unsigned int i = 0; i < sizeof(default_test_sizes) / sizeof(int); i++)
    {
        printf("%d,%.5g\n", default_test_sizes[i], RunTest(default_test_sizes[i], mobile_wait ? ITERATIONS / 5 : ITERATIONS));
        if (mobile_wait) sleep(5);
    }

    return 0;
}

/// <summary>
/// Heuristic to make sure test runs for enough time but not too long
/// </summary>
/// <param name="size_kb">Region size</param>
/// <param name="iterations">base iterations</param>
/// <returns>scaled iterations</returns>
uint64_t scale_iterations(uint32_t size_kb, uint64_t iterations) {
    uint64_t retval = iterations;
    if (size_kb <= 512) retval *= 10;
    if (size_kb > 4096) retval /= 10;
    if (size_kb > 131072) retval /= 30;
    return retval;
}

float RunTest(uint32_t size_kb, uint64_t iterations) {
    #ifdef __linux__
    struct timespec startTs, endTs;
    #else
    struct timeval startTv, endTv;
    #endif

    uint32_t list_size = size_kb * 1024 / 4;
    uint32_t sum = 0, current;

    // Fill list to create random access pattern
    int* A = (int*) malloc(sizeof(int) * list_size);
    for (uint32_t i = 0; i < list_size; i++) {
        A[i] = i;
    }

    int iter = list_size;
    while (iter > 1) {
        iter -= 1;
        int j = iter - 1 == 0 ? 0 : rand() % (iter - 1);
        uint32_t tmp = A[iter];
        A[iter] = A[j];
        A[j] = tmp;
    }

    uint64_t scaled_iterations = scale_iterations(size_kb, iterations);

    // Run test
    #ifdef __linux__
    timespec_get(&startTs, TIME_UTC);
    #else
    gettimeofday(&startTv, NULL);
    #endif

    current = A[0];
    for (uint64_t i = 0; i < scaled_iterations; i++) {
        current = A[current];
        sum += current;
    }
    
    #ifdef __linux__
    timespec_get(&endTs, TIME_UTC);
    uint64_t time_diff_ns = (long) (endTs.tv_sec - startTs.tv_sec) * 1e9 + (endTs.tv_nsec - startTs.tv_nsec);
    #else
    gettimeofday(&endTv, NULL);
    uint64_t time_diff_ns = (1000 * (endTv.tv_sec - startTv.tv_sec) + ((endTv.tv_usec - startTv.tv_usec) / 1000)) * 1e6;
    #endif

    double latency = (double) time_diff_ns / (double) scaled_iterations;
    free(A);

    if (sum == 0) printf("sum == 0 (?)\n");
    return latency;
}

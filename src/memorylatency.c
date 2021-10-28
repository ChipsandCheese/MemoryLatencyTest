#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define ITERATIONS 100000000

#ifdef __i386
typedef float floating_t;
#else
typedef double floating_t;
#endif

int default_test_sizes[37] = { 2, 4, 8, 12, 16, 24, 32, 48, 64, 96, 128, 192, 256, 384, 512, 600, 768, 1024, 1536, 2048,
                               3072, 4096, 5120, 6144, 8192, 10240, 12288, 16384, 24567, 32768, 65536, 98304,
                               131072, 262144, 393216, 524288, 1048576 };

floating_t RunTest(uint32_t size_kb, uint32_t iterations);

int main(int argc, char* argv[]) {
    int maxTestSizeMB = 0;

    int option;
    while ((option = getopt(argc, argv, ":m:i:")) != -1) {
        switch (option) {
            case '?': // unknown option
                fprintf(stderr, "Unknown option '%c' provided.\n", optopt);
                return 1;
            case 'm':
                maxTestSizeMB = atoi(optarg);
                if (maxTestSizeMB <= 0) {
                    fprintf(stderr, "Max test size must be a positive integer.\n");
                    return 1;
                } else {
                    printf("Setting max test size to %u MB.\n", maxTestSizeMB);
                }
        }
    }

    printf("Region,Latency (ns)\n");
    for (long unsigned int i = 0; i < sizeof(default_test_sizes) / sizeof(int); i++) {
        if (maxTestSizeMB == 0 || default_test_sizes[i] <= maxTestSizeMB * 1024) {
            printf("%d,%.5g\n", default_test_sizes[i], RunTest(default_test_sizes[i], ITERATIONS));
        } else {
            printf("Stopping at %d KB.\n", maxTestSizeMB * 1024);
            break;
        }
    }

#ifdef _WIN32
    printf("\nPress the ENTER key to continue...");
    getchar();
#endif

    return 0;
}

// Scales iterations based on region size, to avoid running the test for too long
uint32_t scale_iterations(uint32_t size_kb, uint32_t iterations) {
    return 10 * iterations / pow(size_kb, 1.0 / 4.0);
}

floating_t RunTest(uint32_t size_kb, uint32_t iterations) {
    struct timeval startTv, endTv;
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

    uint32_t scaled_iterations = scale_iterations(size_kb, iterations);

    // Run test
    gettimeofday(&startTv, NULL);
    current = A[0];
    for (uint32_t i = 0; i < scaled_iterations; i++) {
        current = A[current];
        sum += current;
    }
    gettimeofday(&endTv, NULL);
    time_t time_diff_ms = 1e6 * (endTv.tv_sec - startTv.tv_sec) + (endTv.tv_usec - startTv.tv_usec);
    floating_t latency = 1e3 * (floating_t) time_diff_ms / (floating_t) scaled_iterations;
    free(A);

    if (sum == 0) printf("sum == 0 (?)\n");
    return latency;
}

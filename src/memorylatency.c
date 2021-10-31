#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>


#if defined(__i386) || defined(__i686)
int32_t ITERATIONS = 50000000;
typedef float floating_t;
#else
int32_t ITERATIONS = 100000000;
typedef double floating_t;
#endif

#ifdef __x86_64
extern void preplatencyarr(uint64_t *arr, uint32_t len) __attribute__((ms_abi));
extern uint32_t latencytest(uint64_t iterations, uint64_t *arr) __attribute((ms_abi));
#elif defined(__i386) || defined(__i686)
extern void preplatencyarr(uint32_t *arr, uint32_t len) __attribute__((fastcall));
extern uint32_t latencytest(uint32_t iterations, uint32_t *arr) __attribute((fastcall));
#endif

int default_test_sizes[37] = { 2, 4, 8, 12, 16, 24, 32, 48, 64, 96, 128, 192, 256, 384, 512, 600, 768, 1024, 1536, 2048,
                               3072, 4096, 5120, 6144, 8192, 10240, 12288, 16384, 24567, 32768, 65536, 98304,
                               131072, 262144, 393216, 524288, 1048576 };

floating_t RunTest(uint32_t size_kb, uint32_t iterations, bool useAsm);

int main(int argc, char* argv[]) {
    int maxTestSizeMB = 0;
    bool useAsm = false;

#if defined(__x86_64) || defined(__i386) || defined(__i686)
    useAsm = true;
#endif

    printf("Version:\tv%s\n", ver);
    printf("Platform:\t%s\n", plat);
    printf("Architecture:\t%s\n", arch);
    printf("\n");

    int option;
    while ((option = getopt(argc, argv, ":m:i:ch")) != -1) {
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
                    printf("Setting max test size to %d MB.\n", maxTestSizeMB);
                }
                break;
            case 'i':
                ITERATIONS = atoi(optarg);
                if (ITERATIONS <= 0) {
                    fprintf(stderr, "Number of iterations must be a positive integer.\n");
                    return 1;
                } else {
                    printf("Setting base iterations to %d.\n", ITERATIONS);
                }
                break;
            case 'c':
                printf("Running critical path in C.\n");
                useAsm = false;
                break;
            case 'h':
                printf("Usage:\n");
                printf("\t%s [OPTIONS]\n", argv[0]);
                printf("\n");
                printf("Options:\n");
                printf("\t-h\t\tPrints this help text.\n");
                printf("\t-c\t\tForces the C version of the test.\n");
                printf("\t-m <max>\tSets the max test size to the provided number, in megabytes.\n");
                printf("\t-i <iterations>\tSets the base number of iterations to run for each region size.\n");
                return 0;
        }
    }

    printf("Region,Latency (ns)\n");
    for (long unsigned int i = 0; i < sizeof(default_test_sizes) / sizeof(int); i++) {
        if (maxTestSizeMB == 0 || default_test_sizes[i] <= maxTestSizeMB * 1024) {
            floating_t result = RunTest(default_test_sizes[i], ITERATIONS, useAsm);
            if (isnan(result)) {
                printf("Stopping at %d KB.\n", default_test_sizes[i]);
                return 2;
            }
            printf("%d,%.5g\n", default_test_sizes[i], result);
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

#if defined(__i386) || defined(__i686)
#define POINTER_SIZE 4
#define POINTER_INT uint32_t
#else
#define POINTER_SIZE 8
#define POINTER_INT uint64_t
#endif

floating_t RunTest(uint32_t size_kb, uint32_t iterations, bool useAsm) {
    struct timeval startTv, endTv;
    uint32_t list_size = size_kb * 1024 / POINTER_SIZE; // using 32-bit pointers
    uint32_t sum = 0;

    // Fill list to create random access pattern
    POINTER_INT* A = (POINTER_INT*) malloc(POINTER_SIZE* list_size);

    if (!A) {
        fprintf(stderr, "Failed to allocate memory for %u KB test.\n", size_kb);
        return NAN;
    }

    for (uint32_t i = 0; i < list_size; i++) {
        A[i] = i;
    }

    int iter = list_size;
    while (iter > 1) {
        iter -= 1;
        int j = iter - 1 == 0 ? 0 : rand() % (iter - 1);
        POINTER_INT tmp = A[iter];
        A[iter] = A[j];
        A[j] = tmp;
    }

    if (useAsm) {
        preplatencyarr(A, list_size);
    }

    uint32_t scaled_iterations = scale_iterations(size_kb, iterations);

    // Run test
    gettimeofday(&startTv, NULL);

    if (useAsm) {
        sum = latencytest(scaled_iterations, A);
    } else {
        uint32_t current = A[0];
        for (uint32_t i = 0; i < scaled_iterations; i++) {
            current = A[current];
            sum += current;
        }
    }

    gettimeofday(&endTv, NULL);

    time_t time_diff_ms = 1e6 * (endTv.tv_sec - startTv.tv_sec) + (endTv.tv_usec - startTv.tv_usec);
    floating_t latency = 1e3 * (floating_t) time_diff_ms / (floating_t) scaled_iterations;
    free(A);

    if (sum == 0) printf("sum == 0 (?)\n");
    return latency;
}

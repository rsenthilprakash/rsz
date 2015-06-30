#include "AllTests.h"
#include <stdio.h>

static size_t check_count = 0;
void increment_check_count(void)
{
    ++check_count;
}

static size_t failure_count = 0;
void increment_failure_count(void)
{
    ++failure_count;
}

static void run_all_tests(void)
{
    size_t num_groups = sizeof wrappers / sizeof wrappers[0];
    size_t num_tests = 0;

    for (size_t j = 0; j < num_groups; ++j) {
        struct TestsWrapper *wrapper = wrappers[j];

        for (size_t i = 0; i < wrapper->num_tests; ++i) {
            wrapper->setup();
            wrapper->tests[i]();
            wrapper->cleanup();
            printf(".");
            ++num_tests;
        }
    }

    printf("\nTotal tests: %zu, checks: %zu, failed: %zu \n", num_tests, check_count, failure_count);

    if (!failure_count)
        printf("All tests passed\n");
}

int main(void)
{
    run_all_tests();

    return 0;
}

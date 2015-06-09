#include "resizer.h"
#include "test_functions.h"
#include "AllTests.h"

#include <stdio.h>

#define TEST(a, b) void b(void)
#define IGNORE_TEST(a, b) void b(void)

static size_t failure_count = 0;
static size_t check_count = 0;

#define CHECK(cond)                                                  \
do {                                                                 \
    check_count++;                                                   \
    if (!(cond)) {                                                   \
        printf("\n%s: %d " #cond " failed\n", __FILE__, __LINE__);   \
        failure_count++;                                             \
        return;                                                      \
    }                                                                \
} while(0);

static void setup(void)
{
}

static void cleanup(void)
{
}

TEST(dummy, test_scale_factor_for_input_output_dimensions)
{
    CHECK(resizer_get_scale_factor(1920, 1280) == 1.5)
}

static void run_all_tests(void)
{
    size_t total = sizeof test_functions / sizeof test_functions[0];

    for (size_t i = 0; i < total; ++i) {
        setup();
        test_functions[i]();
        cleanup();
        printf(".");
    }

    printf("\nTotal tests: %zu, checks: %zu, failed: %zu \n", total, check_count, failure_count);

    if (!failure_count)
        printf("All tests passed\n");
}

int main(void)
{
    run_all_tests();

    return 0;
}

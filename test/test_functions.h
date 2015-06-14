#ifndef TEST_FUNCTIONS_H_INCLUDED
#define TEST_FUNCTIONS_H_INCLUDED

#include <stddef.h>

typedef void (*test_functions_t)(void);

struct TestsWrapper {
    void (*setup)(void);
    void (*cleanup)(void);
    test_functions_t *tests;
    size_t num_tests;
};

#endif

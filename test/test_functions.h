#ifndef TEST_FUNCTIONS_H_INCLUDED
#define TEST_FUNCTIONS_H_INCLUDED

#include <stddef.h>

typedef void (*TestFunctions)(void);

struct TestsWrapper {
    void (*setup)(void);
    void (*cleanup)(void);
    TestFunctions *tests;
    size_t num_tests;
};

#endif

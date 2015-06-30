#ifndef CHECKS_H_INCLUDED
#define CHECKS_H_INCLUDED

#include <stdio.h>

#define TEST(a, b) void b(void)
#define IGNORE_TEST(a, b) void b(void)

void increment_failure_count(void);
void increment_check_count(void);

#define CHECK(cond)                                                     \
    do {                                                                \
        increment_check_count();                                        \
        if (!(cond)) {                                                  \
            printf("\n%s: %d \n",  __FUNCTION__, __LINE__);             \
            printf(#cond " failed\n");                                  \
            increment_failure_count();                                  \
        }                                                               \
    } while(0)

#define CHECK_EQUALS_UNSIGNED_INT(a, b)                                 \
    do {                                                                \
        increment_check_count();                                        \
        if (a != b) {                                                   \
            printf("\n%s: %d \n",  __FUNCTION__, __LINE__);             \
            printf(#a " == " #b " failed\n");                           \
            printf("Expected: %u Observed: %d \n", a, b);               \
            increment_failure_count();                                  \
        }                                                               \
    } while(0)

#endif

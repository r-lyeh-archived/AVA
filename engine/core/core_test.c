#include <stdio.h>
#include <string.h>

#ifndef TEST
#define TEST(expr, ...) do { \
    int msvc_trick[] = {0,}, ok = !!(expr); \
    char *p = strrchr(__FILE__, '/'), *q = strrchr(__FILE__, '\\'), *r = p > q ? p : p < q ? q : __FILE__; \
    printf("[%s] L%d %s (%s) ", ok ? "pass" : "FAIL", __LINE__, r, #expr); \
    printf("" __VA_ARGS__); puts(""); \
    /* assert(expr); */ \
} while(0)
#endif

#ifdef TEST_DEMO
#include "core_autorun.c"

AUTORUN {
    TEST(0 == 0);
    TEST(0 != 1);
    TEST(1 > 2, "math failed. hello %d", 123);
}

int main()
{}

#endif

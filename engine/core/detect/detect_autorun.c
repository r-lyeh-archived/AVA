// autorun initializers for C
// - rlyeh, public domain
//
// note: based on code by Joe Lowe (public domain).
// note: XIU for C initializers, XCU for C++ initializers, XTU for C deinitializers

#ifndef AUTORUN

#ifdef __cplusplus
#define AUTORUN \
    static void AUTORUN_U(f)(void); \
    static const int AUTORUN_J(AUTORUN_U(f),__1) = (AUTORUN_U(f)(), 1); \
    static void AUTORUN_U(f)(void)
#elif _MSC_VER
#define AUTORUN \
    static void AUTORUN_U(f)(void); \
    static int AUTORUN_J(AUTORUN_U(f),__1) (){ AUTORUN_U(f)(); return 0; } \
    __pragma(section(".CRT$XIU", long, read)) \
    __declspec(allocate(".CRT$XIU")) \
    static int(* AUTORUN_J(AUTORUN_U(f),__2) )() = AUTORUN_J(AUTORUN_U(f),__1); \
    static void AUTORUN_U(f)(void)
#else
#define AUTORUN \
    __attribute__((constructor)) \
    static void AUTORUN_U(f)(void)
#endif

// join + unique macro utils

#define AUTORUN_j(a, b) a##b
#define AUTORUN_J(a, b) AUTORUN_j(a, b)
#define AUTORUN_U(x)    AUTORUN_J(x, __LINE__)

#endif

#ifdef AUTORUN_DEMO
#include <stdio.h>
#include <stdlib.h>

void byebye(void);

AUTORUN {
    puts("before main() #1");
}
AUTORUN {
    puts("before main() #2");
    atexit( byebye );
}

int main() {
    puts("during main()");
}

void byebye(void) {
    puts("after main()");
}

#endif

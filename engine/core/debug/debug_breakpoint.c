void breakpoint();

#ifdef BREAKPOINT_C
#pragma once
#include <signal.h>

void breakpoint() {
#if _MSC_VER
    __debugbreak();    // msvc
#elif __GNUC__
    __builtin_trap();  // gcc and clang
#else
    raise(SIGTRAP);    // posix
#endif
}

#endif

#ifdef BREAKPOINT_DEMO
#include <stdio.h>
int main() {
    puts("trying to invoke debugger... (may crash if debugger is not attached)");
    breakpoint();
}
#endif

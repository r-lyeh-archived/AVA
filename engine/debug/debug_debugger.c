// # debug ---------------------------------------------------------------------

#ifndef DEBUGGER_H
#define DEBUGGER_H

int  debugging();
int  asserting();

#endif

// -----------------------------------------------------------------------------

#ifdef DEBUGGER_C
#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <signal.h>

#if _WIN32
#include <winsock2.h>
#endif



int debugging() {
#if _WIN32
    return IsDebuggerPresent() ? 1 : 0;
#else
    // return ptrace(PTRACE_TRACEME, 0, 0, 0) == -1 ? 1 : 0;
    FILE *fd = fopen("/tmp", "r");
    int rc = fileno(fd) > 5;
    fclose(fd);
    return !!rc;
#endif
}
int asserting() {
    int asserting = 0;
    assert( asserting |= 1 );
    return !!asserting;
}

#endif

#ifdef DEBUGGER_DEMO
#include <stdio.h>
int main() {
    printf("is_debugging: %d\n", debugging());
    printf("is_asserting: %d\n", asserting());
    printf("is_optimized: %d\n", optimized());
}
#endif

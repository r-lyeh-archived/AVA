#ifdef TEST_C
#pragma once

// # tst ######################################################################

// @todo: measure time with overrides as well, so it works as benchmark.
// double (*benchmark)() = 0;
// unit() { now = benchmark(); }
// test() { ... }

#include "debug.c"
#include "debug_logger.c"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static __thread int right = 0, wrong = 0;
static __thread const char *suite = "";

bool unit(const char *name) {
    suite = name;
    right = wrong = 0;
    return true;
}
bool test(int expr) {
    right = right + !!expr;
    wrong = wrong +  !expr;

    char buffer[1024];
    sprintf(buffer, "%s[%s] Unit '%s': test %d/%d %s", // (%5.2f%%)",
        !expr ? "!" : "",
        wrong || !expr ? "FAIL" : " OK ",
        suite,
        right, right+wrong,
        !expr ? "FAILED" : "passed"
        //,100.f * right / (right+wrong)
    );

    LOG(TEST, "%s", buffer);
    return !!expr;
}

#ifdef AUTORUN
AUTORUN {
    if( unit( "first unit" ) ) {
        test( 1 == 1 );
        test( 1 + 10 );
    }
    if( unit( "second unit" ) ) {
        test( 1 < 2 );
        test( 2 > 1 );
    }
    if( unit( "simulate fail") ) {
        test( 10 < 10 ); // <-- simulated fail here
        test( 1 < 2 );
    }
}
#endif

#endif

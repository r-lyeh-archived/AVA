// based on code by niklas gray (likely public domain)
// - rlyeh, public domain

#ifndef QUARK_H
#define QUARK_H

typedef struct array_of_strings {
    char *block;
    uint64_t block_used;
    array(char*) strings;
    unsigned index;
} array_of_strings;

API int   intern(array_of_strings *a, char *s);
API char* quark(array_of_strings *a, unsigned qid);

#endif

// ----------------------------------------------------------------------------

#ifdef QUARK_C
#pragma once

int intern(array_of_strings *a, char *s) {
    uint64_t n = strlen(s) + 1;
    array_push(a->strings, a->block + a->block_used);
    memcpy(a->block + a->block_used, s, n);
    a->block_used += n;
    return ++a->index;
}

char *quark(array_of_strings *a, unsigned qid) {
    return qid > 0 && qid <= a->index ? a->strings[ qid - 1 ] : "";
}

// ----------------------------------------------------------------------------

#ifdef QUARK_DEMO
int main() {
    array_of_strings as = {0};
    as.block = malloc( 4 * 1024 * 1024 );

    intern(&as, "hello");
    intern(&as, "hello world");

    puts( quark(&as, 0) );
    puts( quark(&as, 1) );
    puts( quark(&as, 2) );
    puts( quark(&as, 99) );
}
#endif
#endif

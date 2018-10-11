#pragma once

// Compile time basename. 64 chars max.
// Hint: strlen("string") == sizeof("string")-1
#define SHORTFILE(s) ( (s) + 1 + ( \
    sizeof(s) > 64 && 64[s] == '/' ? 64 : \
    sizeof(s) > 63 && 63[s] == '/' ? 63 : \
    sizeof(s) > 62 && 62[s] == '/' ? 62 : \
    sizeof(s) > 61 && 61[s] == '/' ? 61 : \
    sizeof(s) > 60 && 60[s] == '/' ? 60 : \
    sizeof(s) > 59 && 59[s] == '/' ? 59 : \
    sizeof(s) > 58 && 58[s] == '/' ? 58 : \
    sizeof(s) > 57 && 57[s] == '/' ? 57 : \
    sizeof(s) > 56 && 56[s] == '/' ? 56 : \
    sizeof(s) > 55 && 55[s] == '/' ? 55 : \
    sizeof(s) > 54 && 54[s] == '/' ? 54 : \
    sizeof(s) > 53 && 53[s] == '/' ? 53 : \
    sizeof(s) > 52 && 52[s] == '/' ? 52 : \
    sizeof(s) > 51 && 51[s] == '/' ? 51 : \
    sizeof(s) > 50 && 50[s] == '/' ? 50 : \
    sizeof(s) > 49 && 49[s] == '/' ? 49 : \
    sizeof(s) > 48 && 48[s] == '/' ? 48 : \
    sizeof(s) > 47 && 47[s] == '/' ? 47 : \
    sizeof(s) > 46 && 46[s] == '/' ? 46 : \
    sizeof(s) > 45 && 45[s] == '/' ? 45 : \
    sizeof(s) > 44 && 44[s] == '/' ? 44 : \
    sizeof(s) > 43 && 43[s] == '/' ? 43 : \
    sizeof(s) > 42 && 42[s] == '/' ? 42 : \
    sizeof(s) > 41 && 41[s] == '/' ? 41 : \
    sizeof(s) > 40 && 40[s] == '/' ? 40 : \
    sizeof(s) > 39 && 39[s] == '/' ? 39 : \
    sizeof(s) > 38 && 38[s] == '/' ? 38 : \
    sizeof(s) > 37 && 37[s] == '/' ? 37 : \
    sizeof(s) > 36 && 36[s] == '/' ? 36 : \
    sizeof(s) > 35 && 35[s] == '/' ? 35 : \
    sizeof(s) > 34 && 34[s] == '/' ? 34 : \
    sizeof(s) > 33 && 33[s] == '/' ? 33 : \
    sizeof(s) > 32 && 32[s] == '/' ? 32 : \
    sizeof(s) > 31 && 31[s] == '/' ? 31 : \
    sizeof(s) > 30 && 30[s] == '/' ? 30 : \
    sizeof(s) > 29 && 29[s] == '/' ? 29 : \
    sizeof(s) > 28 && 28[s] == '/' ? 28 : \
    sizeof(s) > 27 && 27[s] == '/' ? 27 : \
    sizeof(s) > 26 && 26[s] == '/' ? 26 : \
    sizeof(s) > 25 && 25[s] == '/' ? 25 : \
    sizeof(s) > 24 && 24[s] == '/' ? 24 : \
    sizeof(s) > 23 && 23[s] == '/' ? 23 : \
    sizeof(s) > 22 && 22[s] == '/' ? 22 : \
    sizeof(s) > 21 && 21[s] == '/' ? 21 : \
    sizeof(s) > 20 && 20[s] == '/' ? 20 : \
    sizeof(s) > 19 && 19[s] == '/' ? 19 : \
    sizeof(s) > 18 && 18[s] == '/' ? 18 : \
    sizeof(s) > 17 && 17[s] == '/' ? 17 : \
    sizeof(s) > 16 && 16[s] == '/' ? 16 : \
    sizeof(s) > 15 && 15[s] == '/' ? 15 : \
    sizeof(s) > 14 && 14[s] == '/' ? 14 : \
    sizeof(s) > 13 && 13[s] == '/' ? 13 : \
    sizeof(s) > 12 && 12[s] == '/' ? 12 : \
    sizeof(s) > 11 && 11[s] == '/' ? 11 : \
    sizeof(s) > 10 && 10[s] == '/' ? 10 : \
    sizeof(s) >  9 &&  9[s] == '/' ?  9 : \
    sizeof(s) >  8 &&  8[s] == '/' ?  8 : \
    sizeof(s) >  7 &&  7[s] == '/' ?  7 : \
    sizeof(s) >  6 &&  6[s] == '/' ?  6 : \
    sizeof(s) >  5 &&  5[s] == '/' ?  5 : \
    sizeof(s) >  4 &&  4[s] == '/' ?  4 : \
    sizeof(s) >  3 &&  3[s] == '/' ?  3 : \
    sizeof(s) >  2 &&  2[s] == '/' ?  2 : -1 ) )

/*
#include <stdio.h>
int main() {
    puts( SHORTFILE(__FILE__) ); 
}
*/
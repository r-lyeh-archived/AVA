int  optimized();

#ifdef BUILD_C
#pragma once

int optimized() {
#if defined(NDEBUG) || defined(_NDEBUG)
    return 1;
#else
    return 0;
#endif
}

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char *build_stamp() {
    //             01234567890
    // __DATE__ -> Jan 23 2018 -> 20180123_       \ 20180123_170151
    // __TIME__ -> 17:01:51    ->         _170151 / YYYYMMDD_hhmmss
    const char *mos = "JanFebMarAprMayJunJulAugSepOctNovDec";
    const char mo[4] = { __DATE__[0], __DATE__[1], __DATE__[2], 0 };
    
    int yy = atoi(__DATE__+7);
    int mm = 1+( strstr(mos, mo) - mos )/3;
    int dd = atoi(__DATE__+4);
    int hh = atoi(__TIME__+0);
    int mn = atoi(__TIME__+3);
    int ss = atoi(__TIME__+6);

    static char buf[64];
    sprintf( buf, "%04d%02d%02d_%02d%02d%02d", yy, mm, dd, hh, mn, ss);
    return buf;
}

const char *build_shortid() {
    //             01234567890
    // __DATE__ -> Jan 23 2018 -> 18023.    \ 18023.686
    // __TIME__ -> 17:10:09    ->      .686 / YYDDD.xxx (D:day[0..365]) (x=90s-fractions[0..999])
    // xxx digit increments every 90s (24h = ~1500 minutes = 90000s -> 1000 x 90s)
    const char *mos = "JanFebMarAprMayJunJulAugSepOctNovDec";
    const char mo[4] = { __DATE__[0], __DATE__[1], __DATE__[2], 0 };
    
    int yy = atoi(__DATE__+7);
    int mm = 1+( strstr(mos, mo) - mos )/3;
    int dd = atoi(__DATE__+4);
    int hh = atoi(__TIME__+0);
    int mn = atoi(__TIME__+3);
    int ss = atoi(__TIME__+6);

    puts(__DATE__ " " __TIME__);
    puts(__TIMESTAMP__);

    static char buf[64];
    sprintf( buf, "%05d.%03d", (yy - 2000) * 1000 + ((mm-1) * 31) + dd, (int)((hh * 60 + mn) / 1.5) );
    return buf;
}

const char *current_stamp() {
    time_t timev = time(NULL);
    struct tm * now = localtime(&timev);

    static char buf[64];
    sprintf(buf, "%04d%02d%02d_%02d%02d%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec );
    return buf;
}

#endif

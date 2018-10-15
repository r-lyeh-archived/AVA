// today() + now()

// clock10 format: printable base10 clock
//
// 18446744073709551615 <-- 64-bit max
// YYYYMMDDhhmmssxxxxxx where "YYYY/MM/DD hh/mm/ss.xxxxxx W" YYYY since 2k, x = microseconds, W = day-of-week

// OR
// xxxxxxx = id_counter instead? :D
//

#include <stdint.h>

// - adjusted system time since unix epoch (in printable base10 clock)
// - unadjusted system time since unix epoch (in printable base10 clock)
// - format clock10 into string, "YYYY-MM-DD hh:mm:ss.nnnnnn X" (nnnnnn microsecs, X day-of-week)

API int64_t  date_gmt_today();
API int64_t  date_ust_today();
API char *   date_format(char out[32], uint64_t rtc10);


#ifdef DATE_C
#pragma once
#if !WINDOWS
#include <sys/time.h>
#endif
#include <time.h>

#if 0
API uint64_t now_us();
#endif

static
uint64_t rtc10(int64_t unixstamp) {
    // Reference: Fliegel, H. F. and van Flandern, T. C. (1968).
    // Communications of the ACM, Vol. 11, No. 10 (October, 1968).
    enum { RTC_EPOCH_JULIAN_DAY = 2440588 }; // January 1st, 1970.
    int64_t yy, mm, dd, l, n;
    l = unixstamp / 86400 + 68569 + RTC_EPOCH_JULIAN_DAY;
    n = 4 * l / 146097;
    l = l - (146097 * n + 3) / 4;
    yy = 4000 * (l + 1) / 1461001;
    l = l - 1461 * yy / 4 + 31;
    mm = 80 * l / 2447;
    dd = l - 2447 * mm / 80;
    l = mm / 11;
    mm = mm + 2 - 12 * l;
    yy = 100 * (n - 49) + yy + l;
    yy -= 2000;

    unixstamp = unixstamp % (24 * 3600);
    int hh = (int)unixstamp / 3600;

    unixstamp = unixstamp % 3600;
    int mn = (int)unixstamp / 60;
    int ss = (int)unixstamp % 60;

#if 1
    static int id = 0;
#else
    int id = (now_us() % 1000000);
#endif

    return
    /// Every base10 calendar is a 64-bit number:
    /// 18446744073709551615
    /// *YYYMMDDhhmmssuuuuuu uuuuuu = microseconds
    yy *   10000000000000000ULL +
    mm *     100000000000000ULL +
    dd *       1000000000000ULL +
    hh *         10000000000ULL +
    mn *           100000000ULL +
    ss *             1000000ULL +
    //                000000ULL;
                           id++;
}

char *date_format( char fmt[32], uint64_t clock10 ) {
    int y, m, d, dow;
    int us = clock10 % 1000000; clock10 /= 1000000;
    int ss = clock10 %  100; clock10 /= 100;
    int mn = clock10 %  100; clock10 /= 100;
    int hh = clock10 %  100; clock10 /= 100; d = clock10 % 100;
    int dd =            d; clock10 /= 100; m = clock10 % 100;
    int mm =            m; clock10 /= 100; y = clock10 % 100;
    int yy =            y; clock10 /= 100;
    if( 1 ) {
        /* Tomohiko Sakamoto's Algorithm */
        /* Get day of the week [0=Sun..6=Sat]*/
        int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
        y -= m < 3;
        dow = (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
    }
    sprintf(fmt, "%04d-%02d-%02d %02d:%02d:%02d.%06d %d",
        2000 + yy, mm, dd, hh, mn, ss, us, dow);
    return fmt;
}

int64_t date_ust_today() {
    return rtc10( time(0) );
}
int64_t date_gmt_today() {
    time_t t = time(0);
    struct tm *gtm = gmtime(&t);
    int hh1 = gtm->tm_hour, mm1 = gtm->tm_min;
    struct tm *ltm = localtime(&t);
    int hh2 = ltm->tm_hour, mm2 = ltm->tm_min;
    int hdiff = hh2-hh1;
    int mdiff = mm2-mm1;
    return rtc10( time(0) + (1) * (hdiff * 3600 + mdiff * 60) );
}

#endif

#ifdef DATE_DEMO
#include <stdio.h>
int main() {
    char buf[32];
    uint64_t ust = date_ust_today(); printf("ust %019llu (today) -> %s\n", ust, date_format(buf, ust) );
    uint64_t gmt = date_gmt_today(); printf("gmt %019llu (today) -> %s\n", gmt, date_format(buf, gmt) );
}
#endif

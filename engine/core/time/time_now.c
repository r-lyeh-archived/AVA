#include <stdint.h>

// - nanoseconds since app initialization
// - microseconds since app initialization
// - milliseconds since app initialization
// - seconds since app initialization
// - minutes since app initialization
// - hours since app initialization

API uint64_t now_ns();
API uint64_t now_us();
API uint64_t now_ms();
API uint64_t now_ss();
API uint64_t now_hh();
API uint64_t now_mm();

// ----------------------------------------------------------------------------

#ifdef NOW_C
#pragma once
#include "../detect/detect.c" // platform, SYS_

#define TIMER_E3 1000ULL
#define TIMER_E6 1000000ULL
#define TIMER_E9 1000000000ULL

#ifdef        CLOCK_MONOTONIC_RAW
#define NOW_MONOTONIC CLOCK_MONOTONIC_RAW
#elif defined CLOCK_MONOTONIC
#define NOW_MONOTONIC CLOCK_MONOTONIC
#else
// #define NOW_MONOTONIC CLOCK_REALTIME // untested
#endif

static uint64_t nanotimer() {
#  if SYS_WIN || SYS_360 || SYS_XB1
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (uint64_t)li.QuadPart;
#elif SYS_PS4
    return (uint64_t)sceKernelReadTsc();
#elif SYS_AND
    return (uint64_t)clock();
#elif defined NOW_MONOTONIC
    struct timespec ts;
    clock_gettime(NOW_MONOTONIC, &ts);
    return (TIMER_E9 * (uint64_t)ts.tv_sec) + ts.tv_nsec;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (TIMER_E6 * (uint64_t)tv.tv_sec) + tv.tv_usec;
#endif
}

static uint64_t nanofreq() {
#  if SYS_WIN || SYS_360 || SYS_XB1
    LARGE_INTEGER li;
    QueryPerformanceFrequency(&li);
    return li.QuadPart;
#elif SYS_PS4
    return sceKernelGetTscFrequency();
#elif SYS_AND
    return CLOCKS_PER_SEC;
#elif defined NOW_MONOTONIC
    return TIMER_E9;
#else
    return TIMER_E6;
#endif
}

// [ref] https://github.com/rust-lang/rust/blob/3809bbf47c8557bd149b3e52ceb47434ca8378d5/src/libstd/sys_common/mod.rs#L124
// Computes (a*b)/c without overflow, as long as both (a*b) and the overall result fit into 64-bits.
static
uint64_t now_muldiv64(uint64_t a, uint64_t b, uint64_t c) {
    uint64_t q = a / c;
    uint64_t r = a % c;
    return q * b + r * b / c;
}

uint64_t now_ns() {
    static uint64_t epoch = 0;
    static uint64_t freq = 0;
    if( !epoch ) {
        epoch = nanotimer();
        freq = nanofreq();
    }
    return (uint64_t)now_muldiv64(nanotimer() - epoch, TIMER_E9, freq);
}
uint64_t now_us() {
    return now_ns() / TIMER_E3;
}
uint64_t now_ms() {
    return now_ns() / TIMER_E6;
}
uint64_t now_ss() {
    return now_ns() / TIMER_E9;
}
uint64_t now_mm() {
    return now_ss() / 60;
}
uint64_t now_hh() {
    return now_mm() / 60;
}

/* untested: */
uint64_t time_raw() {
    return nanotimer();
}
/* untested: */
double time_diff( uint64_t raw1, uint64_t raw2 ) {
    static double freq = 0; if(!freq) freq = 1.0 / nanofreq();
    return (raw1 < raw2 ? raw2 - raw1 : raw1 - raw2) * freq;

    //uint64_t ts = now_muldiv64(raw1 < raw2 ? raw2 - raw1 : raw1 - raw2, TIMER_E9, nanofreq());
    //return ts * 1.0e-9;
}


#endif


#ifdef NOW_DEMO
#include <stdio.h>
#include <stdlib.h>
int main() {
    uint64_t timer1 = now_ns();
    puts("hello world");
    timer1 = now_ns() - timer1;

    printf("print took %lluns\n", timer1);
    system("pause");
    printf("%02llu hours, %02llu minutes, %02llu seconds since app start\n", now_hh(), now_mm(), now_ss());


    uint64_t timer2a = time_raw();
    puts("hello world");
    uint64_t timer2b = time_raw();
    printf("%llu -> %llu = %fs", timer2a, timer2b, time_diff( timer2a, timer2b ) );
}
#endif

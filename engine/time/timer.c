uint64_t time_hh();
uint64_t time_mm();
uint64_t time_ss();
uint64_t time_ms();
uint64_t time_us();
uint64_t time_ns();

#ifdef TIMER_C
#pragma once

#define TIMER_1E3 1000ULL
#define TIMER_1E6 1000000ULL
#define TIMER_1E9 1000000000ULL

#if defined(CLOCK_MONOTONIC_RAW) && !SYS_WIN
#define MY_CLOCK_MONOTONIC CLOCK_MONOTONIC_RAW
#elif defined(CLOCK_MONOTONIC) && !SYS_WIN
#define MY_CLOCK_MONOTONIC CLOCK_MONOTONIC
#endif

static uint64_t time_raw() {
#  if SYS_WIN || SYS_360 || SYS_XB1
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (uint64_t)li.QuadPart;
#elif SYS_PS4
    return (uint64_t)sceKernelReadTsc();
#elif SYS_AND
    return (uint64_t)clock();
#elif defined(MY_CLOCK_MONOTONIC)
    struct timespec ts;
    clock_gettime(MY_CLOCK_MONOTONIC, &ts);
    return (TIMER_1E9 * (uint64_t)ts.tv_sec) + ts.tv_nsec;
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (TIMER_1E6 * (uint64_t)tv.tv_sec) + tv.tv_usec;
#endif
}

static uint64_t time_freq() {
#  if SYS_WIN || SYS_360 || SYS_XB1
    LARGE_INTEGER li;
    QueryPerformanceFrequency(&li);
    return li.QuadPart;
#elif SYS_PS4
    return sceKernelGetTscFrequency();
#elif SYS_AND
    return CLOCKS_PER_SEC;
#elif defined MY_CLOCK_MONOTONIC
    return TIMER_1E9;
#else
    return TIMER_1E6;
#endif
}

uint64_t time_ns() {
    static uint64_t epoch = 0;
    static uint64_t freq = 0;
    if( !epoch ) {
        epoch = time_raw();
        freq = TIMER_1E9 / time_freq();
    }
    return (uint64_t)( time_raw() - epoch ) * freq;
}
uint64_t time_us() {
    return time_ns() / TIMER_1E3;
}
uint64_t time_ms() {
    return time_ns() / TIMER_1E6;
}
uint64_t time_ss() {
    return time_ns() / TIMER_1E9;
}
uint64_t time_mm() {
    return time_ss() / 60;
}
uint64_t time_hh() {
    return time_mm() / 60;
}

#endif

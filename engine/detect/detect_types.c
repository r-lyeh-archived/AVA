#ifndef TYPES_H
#define TYPES_H
#include <stdint.h>

// types

typedef unsigned usize;    // 32bit size_t
typedef int      isize;    // 32bit ssize_t

#ifdef _MSC_VER
typedef ptrdiff_t ssize_t; // 64bit ssize_t for MSC
#endif

// units and constants

#define u_1e9 *1000000000ULL
#define u_1e6 *1000000ULL
#define u_1e3 *1000ULL

#define u_TiB *1099511627776ULL
#define u_GiB *1073741824ULL
#define u_MiB *1048576ULL
#define u_KiB *1024ULL

#define u_ss *1000000000LL
#define u_ms *1000000LL
#define u_us *1000LL
#define u_ns *1LL

#define u_Km *1000000LL
#define u_m  *1000LL
#define u_ft *304.8
#define u_dm *100LL
#define u_cm *10LL
#define u_mm *1LL

#define u_kg *1000LL
#define u_lb *453.59237
#define u_g  *1LL

#define u_degrees  *(3.14159f/180.f)
#define u_radians  *1.f //(180.f/3.14159f)

#define u_bytes *1ULL
#define u_bits  *0.125

#endif

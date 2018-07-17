// units and measures

#ifndef UNITS_H
#define UNITS_H

#include <stdint.h>

#if 0
static const uint64_t   u_1e3 = 1000ULL;              // large integer 1e3
static const uint64_t   u_1e6 = 1000*1000ULL;         // large integer 1e6
static const uint64_t   u_1e9 = 1000*1000*1000ULL;    // large integer 1e9
static const   double     u_g = 1, u_lb = 453.59237;  // grams and pounds
static const   double     u_m = 1, u_ft = 3.2808399;  // meters and feet
static const   double     u_B = 1, u_b  = 0.125;      // bytes and bits

#define Kilo *1000LL
#define Mega *1000*1000LL
#define Giga *1000*1000*1000LL
#define Tera *1000*1000*1000*1000LL
#define Kibi *1024LL
#define Mebi *1024*1024LL
#define Gibi *1024*1024*1024LL
#define Tebi *1024*1024*1024*1024LL
#define Byte *1
#define Bit  /8
#endif

#endif

#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>        // u/int{8,16,32,64}
#include <stdio.h>         // size_t
#include <sys/types.h>     // off_t

// types

typedef unsigned usize;    // 32bit size_t
typedef int      isize;    // 32bit ssize_t

#ifdef _MSC_VER
typedef ptrdiff_t ssize_t; // 64bit ssize_t for MSC
//typedef ssize_t   off_t;   // posix
#endif

// units and constants

#define E9 *1000000000ULL
#define E6 *1000000ULL
#define E3 *1000ULL

#define TIB *1099511627776ULL
#define GIB *1073741824ULL
#define MIB *1048576ULL
#define KIB *1024ULL

#define SS *1000000000LL
#define MS *1000000LL
#define US *1000LL
#define NS *1LL

#define KM *1000000LL
#define MT *1000LL
#define FT *304.8
#define DM *100LL
#define CM *10LL
#define MM *1LL

#define KG *1000LL
#define LB *453.59237
#define GR *1LL

#define DEGREES  *(3.14159f/180.f)
#define RADIANS  *1.f //(180.f/3.14159f)

#endif

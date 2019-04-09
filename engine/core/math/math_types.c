// math types. for storage purposes only.

#ifndef MTYPES_H
#define MTYPES_H
#include <stdint.h>
#include "math_linear.c" // M_CAST

typedef struct byte2 { uint8_t x,y;     } byte2;
typedef struct byte3 { uint8_t x,y,z;   } byte3;
typedef struct byte4 { uint8_t x,y,z,w; } byte4;

typedef struct int2 { int x,y;     } int2;
typedef struct int3 { int x,y,z;   } int3;
typedef struct int4 { int x,y,z,w; } int4;

typedef struct float2 { float x,y;     } float2;
typedef struct float3 { float x,y,z;   } float3;
typedef struct float4 { float x,y,z,w; } float4;

typedef struct double2 { double x,y;     } double2;
typedef struct double3 { double x,y,z;   } double3;
typedef struct double4 { double x,y,z,w; } double4;

#define byte2(x,y)       M_CAST(byte2, (uint8_t)(x), (uint8_t)(y) )
#define byte3(x,y,z)     M_CAST(byte3, (uint8_t)(x), (uint8_t)(y), (uint8_t)(z) )
#define byte4(x,y,z,w)   M_CAST(byte4, (uint8_t)(x), (uint8_t)(y), (uint8_t)(z), (uint8_t)(w) )

#define int2(x,y)        M_CAST(int2, (int)(x), (int)(y) )
#define int3(x,y,z)      M_CAST(int3, (int)(x), (int)(y), (int)(z) )
#define int4(x,y,z,w)    M_CAST(int4, (int)(x), (int)(y), (int)(z), (int)(w) )

#define float2(x,y)      M_CAST(float2, (float)(x), (float)(y) )
#define float3(x,y,z)    M_CAST(float3, (float)(x), (float)(y), (float)(z) )
#define float4(x,y,z,w)  M_CAST(float4, (float)(x), (float)(y), (float)(z), (float)(w) )

#define double2(x,y)     M_CAST(double2, (double)(x), (double)(y) )
#define double3(x,y,z)   M_CAST(double3, (double)(x), (double)(y), (double)(z) )
#define double4(x,y,z,w) M_CAST(double4, (double)(x), (double)(y), (double)(z), (double)(w) )

#endif

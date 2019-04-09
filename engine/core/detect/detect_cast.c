// simple C constructors
// - rlyeh, public domain

#pragma once
#ifdef __cplusplus
#define CAST(type, ...)  ( type { __VA_ARGS__ } )
#else
#define CAST(type, ...)  ((type){ __VA_ARGS__ } )
#endif

#if 0

typedef struct vec4 {
    float x,y,z,w;
} vec4;

#define vec4(x, y, z, w) CAST(vec4, (x), (y), (z), (w))
#define vec3(x, y, z   ) CAST(vec4, (x), (y), (z),    )
#define vec2(x, y      ) CAST(vec4, (x), (y),         )

#include <stdio.h>

void print( vec4 v ) {
    printf("%f %f %f %f\n", v.x, v.y, v.z, v.w);
}

int main() {
    print(vec2(1,2));
    print(vec3(1,2,3));
    print(vec4(1,2,3,4));

    vec4 v = vec2(123,456);
    print(v);
}

#endif

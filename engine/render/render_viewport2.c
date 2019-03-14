/*
    Splits screen viewports.
    Given screen coordinate that breaks viewport into 4 slots.

    >     +-------+-------+ P(x,y) split at width:50%, height: 50%
    >     |       |       |      
    >     |   0   |   1   |      
    >     |       |       |      
    >     +-------P-------+      
    >     |       |       |      
    >     |   2   |   3   |      
    >     |       |       |      
    >     +-------+-------+      
*/

#ifndef VIEWPORT2_H
#define VIEWPORT2_H

API void viewport_color(uint8_t r, uint8_t g, uint8_t b);

API void viewport_split( float width_delta, float height_delta );
API void viewport_change( int slot );

#endif

#ifdef VIEWPORT2_C
#pragma once

#include "render_opengl.c"
#include "engine_minimal.c" // math_linear

typedef struct vec4b {
    float x,y,z,w;
} vec4b;

#define vec4b(x, y, z, w) CAST(vec4b, (x), (y), (z), (w))

static vec4b viewport[4];

void viewport_split( float width_delta, float height_delta ) {
    int *rect = window_size();
    int screen_width = rect[0], screen_height = rect[1];
    int split_x = rect[0] * width_delta, split_y = rect[1] * height_delta;

    // four region slots, vec4(left,bottom,width,height) each.
    // top-left orientation.

    viewport[0] = vec4b( 0,             0,              split_x-1,               split_y-1 );
    viewport[1] = vec4b( split_x,       0, screen_width-split_x-1,               split_y-1 );
    viewport[2] = vec4b( 0,       split_y,              split_x-1, screen_height-split_y-1 );
    viewport[3] = vec4b( split_x, split_y, screen_width-split_x-1, screen_height-split_y-1 );
}
void viewport_color(uint8_t r, uint8_t g, uint8_t b) {
    glClearColor(r/255.0, g/255.0, b/255.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}
void viewport_change( int slot ) {
    float sh = window_size()[1];
    float x = viewport[slot&3].x;
    float y = viewport[slot&3].y;
    float w = viewport[slot&3].z;
    float h = viewport[slot&3].w;

    // glViewport(x, y, w, h); // bottom-left
    glViewport(x, sh-y-h, w, h); // top-left
}
/*
bool viewport_rect( int slot ) {
    // rect present if w*h > 0. useful function?
    return (viewport[slot&3].z * viewport[slot&3].w) > 0;
}
*/

#endif

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
API void viewport_clear( int color, int depth );
API void viewport_select( int slot, float width_delta, float height_delta );

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

void viewport_select( int slot, float width_delta, float height_delta ) {
    int *rect = window_size();
    int screen_width = rect[0], screen_height = rect[1];
    int split_x = rect[0] * width_delta, split_y = rect[1] * height_delta;

    // four region slots, vec4(left,bottom,width,height) each.
    // top-left orientation.

    viewport[0] = vec4b( 0,             0,              split_x-1,               split_y-1 );
    viewport[1] = vec4b( split_x,       0, screen_width-split_x-1,               split_y-1 );
    viewport[2] = vec4b( 0,       split_y,              split_x-1, screen_height-split_y-1 );
    viewport[3] = vec4b( split_x, split_y, screen_width-split_x-1, screen_height-split_y-1 );

    // apply

    float sh = window_size()[1];
    float x = viewport[slot&3].x;
    float y = viewport[slot&3].y;
    float w = viewport[slot&3].z;
    float h = viewport[slot&3].w;

    // top-left
    glViewport(x, sh-y-h, w, h); 

    // bottom-left
    // glViewport(x, y, w, h)

    // ideas
    // if( flags & (TOPLEFT|UNIT))          ortho44(proj, -1, +1, -1, +1, -1, 1 );       // 0,0 top-left, 1-1 bottom-right
    // if( flags & (TOPLEFT|RESOLUTION))    ortho44(proj, -w/2, w/2, h/2, -h/2, -1, 1 ); // 0,0 top-left, w-h bottom-right
    // if( flags & (BOTTOMLEFT|RESOLUTION)) ortho44(proj, 0, w, 0, h, -1, 1 );           // 0,0 bottom-left, w-h top-right
}
void viewport_color(uint8_t r, uint8_t g, uint8_t b) {
    glClearColor(r/255.0, g/255.0, b/255.0, 1.0);
}
void viewport_clear(int color, int depth) {
    glClear((color ? GL_COLOR_BUFFER_BIT : 0) | (depth ? GL_DEPTH_BUFFER_BIT : 0));
}

#endif

/*
    Split-screen viewports.
    Given screen coordinate breaks viewport into 4 slots.

    +-------+-------+ P(x,y) splits viewport at width:50%, height: 50%
    |       |       | It creates 4 slots (0,1,2,3) after split.
    |   0   |   1   | 
    |       |       | 
    +-------P-------+ 
    |       |       | 
    |   2   |   3   | 
    |       |       | 
    +-------+-------+ 
*/

#ifndef VIEWPORT2_H
#define VIEWPORT2_H

API void viewport_clear(vec3 color, bool depth);
API void viewport_select(int slot, vec2 split_point);

#endif

#ifdef VIEWPORT2_C
#pragma once

#include "render_opengl.c"
#include "engine_minimal.c" // math_linear

void viewport_clear(vec3 color3, bool depth) {
    float luma = color3.x + color3.y + color3.z;
    glClearColor(color3.x, color3.y, color3.z, 1);
    glClear((luma > 0 ? GL_COLOR_BUFFER_BIT : 0) | (depth ? GL_DEPTH_BUFFER_BIT : 0));
}

void viewport_select(int slot, vec2 split) {
    int *rect = window_size(), screen_width = rect[0], screen_height = rect[1];

    // four region slots, vec4(left,bottom,width,height) each.
    // top-left orientation.

    vec4 viewport[4];
    viewport[0] = vec4( 0,             0,              split.x,               split.y );
    viewport[1] = vec4( split.x,       0, screen_width-split.x,               split.y );
    viewport[2] = vec4( 0,       split.y,              split.x, screen_height-split.y );
    viewport[3] = vec4( split.x, split.y, screen_width-split.x, screen_height-split.y );

    // apply

    float x = viewport[slot&3].x;
    float y = viewport[slot&3].y;
    float w = viewport[slot&3].z;
    float h = viewport[slot&3].w;

    // top-left
    glViewport(x, screen_height-y-h, w, h); 
    // glOrtho(0,w,h,0,0,1);

    // bottom-left
    // glViewport(x, y, w, h)
    // glOrtho(0,w,0,h,0,1);

    // ideas
    // if( flags & (TOPLEFT|UNIT))          ortho44(proj, -1, +1, -1, +1, -1, 1 );       // 0,0 top-left, 1-1 bottom-right
    // if( flags & (TOPLEFT|RESOLUTION))    ortho44(proj, -w/2, w/2, h/2, -h/2, -1, 1 ); // 0,0 top-left, w-h bottom-right
    // if( flags & (BOTTOMLEFT|RESOLUTION)) ortho44(proj, 0, w, 0, h, -1, 1 );           // 0,0 bottom-left, w-h top-right
}

#endif

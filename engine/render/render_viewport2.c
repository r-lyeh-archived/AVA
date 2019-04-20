#ifndef VIEWPORT2_H
#define VIEWPORT2_H

API void viewport_color(vec3 color);
API void viewport_clear(bool color, bool depth);
API void viewport_clip(vec2 from, vec2 to);

#endif

#ifdef VIEWPORT2_C
#pragma once

static
void viewport(float x, float y, float w, float h) {
    // top-left
    glViewport(x, window_height()-y-h, w, h); 

    // bottom-left
    // glViewport(x, y, w, h)

    // ideas
    // if( flags & (TOPLEFT|UNIT))          ortho44(proj, -1, +1, -1, +1, -1, 1 );       // 0,0 top-left, 1-1 bottom-right
    // if( flags & (TOPLEFT|RESOLUTION))    ortho44(proj, -w/2, w/2, h/2, -h/2, -1, 1 ); // 0,0 top-left, w-h bottom-right
    // if( flags & (BOTTOMLEFT|RESOLUTION)) ortho44(proj, 0, w, 0, h, -1, 1 );           // 0,0 bottom-left, w-h top-right
}

void viewport_color(vec3 color3) {
    glClearColor(color3.x, color3.y, color3.z, 1);
}

void viewport_clear(bool color, bool depth) {
    glClear((color ? GL_COLOR_BUFFER_BIT : 0) | (depth ? GL_DEPTH_BUFFER_BIT : 0));
}

void viewport_clip(vec2 from, vec2 to) {
    viewport(from.x,from.y,to.x-from.x,to.y-from.y);
}

#endif

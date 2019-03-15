// based on code by @glampert (public domain).
// - rlyeh, public domain.
//
// @todo: split into ddraw_render2d and ddraw_render3d ?

#ifndef DDRAW_H
#define DDRAW_H

// add functions
API void ddraw_aabb(vec3 minbb, vec3 maxbb);
API void ddraw_arrow(vec3 from, vec3 to);
API void ddraw_axis(vec3 center, float radius);
API void ddraw_box(vec3 center, float width, float height, float depth); //void ddraw_box_(vec3 points[8]); // instead?
API void ddraw_circle(vec3 center, vec3 dir, float radius);
API void ddraw_cone(vec3 pos1, vec3 pos2, float radius);
API void ddraw_cross(vec3 center, float radius);
API void ddraw_frustum(float projview_matrix[16]);
API void ddraw_grid(int hcols, int hrows, float step);
API void ddraw_line(vec3 from, vec3 to); //void ddraw_lineEx( vec3 src, vec3 dst, float srccolor[4], float dstcolor[4] );
API void ddraw_normal(vec3 center, vec3 dir, float radius);
API void ddraw_ring(vec3 center, vec3 dir, float radius);
API void ddraw_sphere(vec3 center, float radius);
API void ddraw_sphere2(vec3 center, float radius);
API void ddraw_xzgrid(float mins, float maxs, float y, float step);

API  int ddraw_menu( int submenu_id, const char **lines );
API void ddraw_text2d( vec2 center, const char *fmt, ... );
API void ddraw_text3d( vec3 center, const char *fmt, ... );
API void ddraw_console( const char *fmt, ... );
API void ddraw_render2d( /*int shader, int camera*/ float *projmatrix, float *viewmatrix );

// void pushScale(x);
// void popScale();
// void pushExpire(ms);
// void popExpire();
// void pushDepth(on/off);
// void popDepth();

// void ddraw_point(vec3 location);
// void ddraw_text2d(vec3 location, const char *text);
// void ddraw_text3d(vec3 location, const char *text);
// 
// void ddraw_enable(int debugMode);

void ddraw_color(float r, float g, float b, float a);

void ddraw_begin(float projview[16]);
void ddraw_end(void);

void ddraw_triangle(vec3 p, vec3 q, vec3 r);
void ddraw_quad(vec3 a, vec3 b, vec3 c, vec3 d);
//void ddraw_rect(float a[2], float b[2]);
//void ddraw_box_(float pvec3[8]);

void ddraw_lineEx( vec3 src, vec3 dst, vec4 col );

#endif

// ----------------------------------------------------------------------------

#ifdef DDRAW_C
#pragma once
#include "render_renderer.c"
#include "render_text.c"
#include "render_font.c"
#include "engine.h" // app/window
#include "render_ddraw.c"

// find/compute basis/triad/axes from single normal

static void basis3(vec3 *tangent, vec3 *bitangent, const vec3 normal) {
    // perp_vector(tangent, normal)
    if( normal.z * normal.z < normal.x * normal.x ) {
        *tangent = vec3(normal.y,-normal.x,0);
    } else {
        *tangent = vec3(0,-normal.z,normal.y);
    }

    *bitangent = cross3(*tangent, normal);
}

// 

static float matTransformPointXYZW2(vec3 *out, const vec3 in, const float m[16]) {
    out->x = (m[0] * in.x) + (m[4] * in.y) + (m[ 8] * in.z) + m[12]; // in.w (vec4) assumed to be 1
    out->y = (m[1] * in.x) + (m[5] * in.y) + (m[ 9] * in.z) + m[13];
    out->z = (m[2] * in.x) + (m[6] * in.y) + (m[10] * in.z) + m[14];
    return  (m[3] * in.x) + (m[7] * in.y) + (m[11] * in.z) + m[15]; // rw
}

// ----------------------------------------------------------------------------

// base colors: RGB, CMY, B/W

static const vec4 red = { 1.0f, 0.0f, 0.0f, 1.0f };
static const vec4 green = { 0.0f, 1.0f, 0.0f, 1.0f };
static const vec4 blue = { 0.0f, 0.0f, 1.0f, 1.0f };

static const vec4 cyan = { 0.0f, 1.0f, 1.f, 1.0f };
static const vec4 magenta = { 1.0f, 0.0f, 1.f, 1.0f };
static const vec4 yellow = { 1.0f, 1.0f, 0.f, 1.0f };

static const vec4 black = { 0.0f, 0.0f, 0.0f, 1.0f };
static const vec4 gray = { 0.3f, 0.3f, 0.3f, 0.6f };
static const vec4 white = { 1.0f, 1.0f, 1.0f, 1.0f};

// ----------------------------------------------------------------------------

enum {
    ORTHO_TOPLEFT = 1,
    ORTHO_CENTERED = 2,
    ORTHO_BOTTOMLEFT = 4,
    //
    ORTHO_NORMALIZED = 8,
};

void ortho(mat44 proj, int flags) {
    // void ortho4(mat44 M, float l, float r, float b, float t, float n, float f);
    // @todo: test all configurations
    int *rect = window_size();
    float w = rect[0], h = rect[1] + 1;
    if( flags & ORTHO_CENTERED ) {
        if( flags & ORTHO_NORMALIZED ) {
            // (-1,-1) top-left, (+1,+1) bottom-right
            ortho44(proj, -1, +1, -1, +1, -1,1 ); 
        } else {
            // (-w/2,-h/2) top-left, (0,0) center, (+w/2,+h/2) bottom-right
            ortho44(proj, -w/2,w/2,-h/2,h/2, -1,1 ); 
        }
    }
    if( flags & ORTHO_TOPLEFT ) {
        if( flags & ORTHO_NORMALIZED ) {
            // (0,0) top-left, (+1,+1) bottom-right
            ortho44(proj, -1, +1, -1, +1, -1,1 );
            translate44(proj, -1, +1, 0 ); 
        } else {
            // (0,0) top-left, (w,h) bottom-right
            ortho44(proj, 0,w,h,0, -1,1 );
        }
    }
    if( flags & ORTHO_BOTTOMLEFT ) {
        if( flags & ORTHO_NORMALIZED ) {
            // (0,0) bottom-left, (+1,+1) top-right
            ortho44(proj, -1, +1, +1, -1, -1,1 );
            translate44(proj, -1, -1, 0 ); 
        } else {
            // (0,0) bottom-left, (w,h) top-right
            ortho44(proj, 0,w,0,h, -1,1 );
        }
    }
}

// -----------------------------------------------------------------------------

// shaders

static const char *ddraw_shader_vtxpc_vs =
    VS130
    "/* render_ddraw.c */\n"
    "/* project view matrix */\n"
    "uniform mat4 projview;\n"
    "/* stream buffer data: */\n"
    "layout (location = 0) in vec4 att_position;\n"
    "layout (location = 1) in vec4 att_color;\n"
    "/* transform vertex and transfer color */\n"
    "out vec4 var_color;\n"
    "void main() {\n"
    "   gl_Position = projview * att_position;\n"
    "   gl_PointSize = 4.f;\n" // att_position.w?
    "   var_color = att_color;\n"
    "}\n";

static const char *ddraw_shader_vtxpc_fs =
    FS130
    "/* render_ddraw.c */\n"
    "/* input vertex color */\n"
    "in vec4 var_color;\n"
    "void main() {\n"
    "   fragColor = var_color;\n"
    "}\n";

// ----------------------------------------------------------------------------

enum {
    DRAW_LINES, // to material?
};

/*
 * Line and rect drawing.
 */

#define MAXBUFFER (4096) // 512*3)   /* size of our triangle buffer */

static unsigned int draw_vao = 0;
static unsigned int draw_vbo = 0;
static int draw_buf_len = 0;
static struct vertex_p3c4_ { vec3 position; vec4 color; } draw_buf[MAXBUFFER];

static vec4 draw_color = { 1, 1, 1, 1 };
static int draw_kind = GL_LINES;

void ddraw_color(float r, float g, float b, float a) {
    draw_color.r = r;
    draw_color.g = g;
    draw_color.b = b;
    draw_color.a = a;
}

void ddraw_begin(float projview[16]) {
    static int draw_prog = 0;
    static int draw_uni_projview = -1;

    if (!draw_prog) {
        draw_prog = shader(ddraw_shader_vtxpc_vs, ddraw_shader_vtxpc_fs);
        draw_uni_projview = glGetUniformLocation(draw_prog, "projview");
    }

    if (!draw_vao) {
        glGenVertexArrays(1, &draw_vao);
        glBindVertexArray(draw_vao);

        glGenBuffers(1, &draw_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, draw_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof draw_buf, NULL, GL_STREAM_DRAW);

        glEnableVertexAttribArray(ATT_POSITION);
        glEnableVertexAttribArray(ATT_COLOR);
        glVertexAttribPointer(ATT_POSITION, 3, GL_FLOAT, 0, sizeof draw_buf[0], (void*)0);
        glVertexAttribPointer(ATT_COLOR, 4, GL_FLOAT, 0, sizeof draw_buf[0], (void*)12);

        glBindVertexArray(0);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE); // additive: 1 * src_color + 1 * fb_color
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // decal: src_color.a * src_color + (1 - src_color.a) * fb_color
    //glBlendFunc(GL_DST_COLOR, GL_ZERO); // modulate: fb_color * src_color + 0 * fb_color

    glUseProgram(draw_prog);
    glUniformMatrix4fv(draw_uni_projview, 1, 0, projview);

    glBindVertexArray(draw_vao);
}

static void ddraw_flush(void) {
    if (draw_buf_len > 0) {
        glBindVertexArray(draw_vao);
        glBindBuffer(GL_ARRAY_BUFFER, draw_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof draw_buf[0] * draw_buf_len, draw_buf);
        glDrawArrays(draw_kind, 0, draw_buf_len);
        draw_buf_len = 0;
    }
}

void ddraw_end(void) {
    ddraw_flush();
    glDisable(GL_BLEND);
}

static void ddraw_vertexEx(vec3 pos, vec4 draw_color) {
    draw_buf[draw_buf_len].position = pos;
    draw_buf[draw_buf_len].color = draw_color;
    draw_buf_len++;
}

static void ddraw_vertex(vec3 pos) {
    ddraw_vertexEx(pos, draw_color);
}

void ddraw_line(vec3 p, vec3 q) {
    if( draw_kind != GL_LINES || draw_buf_len + 2 >= MAXBUFFER ) {
        ddraw_flush();
    }

    draw_kind = GL_LINES;
    ddraw_vertex(p);
    ddraw_vertex(q);
}

void ddraw_triangle(vec3 p, vec3 q, vec3 r) {
    if( draw_kind != GL_TRIANGLES || draw_buf_len + 3 >= MAXBUFFER ) {
        ddraw_flush();
    }

    draw_kind = GL_TRIANGLES;
    ddraw_vertex(p);
    ddraw_vertex(q);
    ddraw_vertex(r);
}

void ddraw_quad(vec3 a, vec3 b, vec3 c, vec3 d) {
    if( draw_kind != GL_TRIANGLES || draw_buf_len + 6 >= MAXBUFFER ) {
        ddraw_flush();
    }

    draw_kind = GL_TRIANGLES;
    ddraw_vertex(a);
    ddraw_vertex(b);
    ddraw_vertex(c);
    ddraw_vertex(a);
    ddraw_vertex(c);
    ddraw_vertex(d);
}

/*
static void ddraw_vertex3(float x, float y, float z){
    vec3 p = {x,y,z};
    ddraw_vertex(p);
}

void ddraw_rect(vec2 a, vec2 b) {
    if( draw_kind != GL_TRIANGLES || draw_buf_len + 6 >= MAXBUFFER ) {
        ddraw_flush();
    }

    draw_kind = GL_TRIANGLES;
    ddraw_vertex3(x[a], y[a], 0);
    ddraw_vertex3(x[a], y[b], 0);
    ddraw_vertex3(x[b], y[b], 0);
    ddraw_vertex3(x[a], y[a], 0);
    ddraw_vertex3(x[b], y[b], 0);
    ddraw_vertex3(x[b], y[a], 0);
}
*/

// ----------------------------------------------------------------------------

#define DDRAW_INIT(NUM_LINES) do { \
    int num_vertices = NUM_LINES * 2; \
    if( draw_kind != GL_TRIANGLES || (draw_buf_len + num_vertices >= MAXBUFFER)) { \
        ddraw_flush(); \
    } \
} while(0)
#define DDRAW_CALC(min,max,step) ((max + 1 - min) / step)

static
void DDRAW_LINE( vec3 src, vec3 dst, vec4 color ) {
    ddraw_vertexEx( src, color );
    ddraw_vertexEx( dst ,color );
}


void ddraw_lineEx( vec3 src, vec3 dst, vec4 col ) {
    DDRAW_INIT(1);
    DDRAW_LINE(src, dst, col);
}

/*
void ddraw_line( vec3 src, vec3 dst ) {
    DDRAW_INIT(1);
    DDRAW_LINE(src, dst, yellow );
}
*/

void ddraw_grid(int hcols, int hrows, float step) {
    int half_rows = hrows;
    int half_cols = hcols;
    int cols = half_cols * 2;
    int rows = half_rows * 2;
    float size_w = step * cols;
    float size_h = step * rows;
    float half_size_w = size_w * 0.5;
    float half_size_h = size_h * 0.5;
    float z = 0.0f;
    float x = 0.0f;

    DDRAW_INIT( (cols+rows)*2 + 5 );

    /* Column lines and axis. */
    for (int i = 1; i < cols; ++i) {
        x = -half_size_w + i * step;

        if (i != half_cols) {
            DDRAW_LINE(vec3(x, 0.0f, -half_size_h), vec3(x, 0.0f, half_size_h), gray);
        } else {
            /* Z-axis. */
            DDRAW_LINE(vec3(x, 0.0f, 0.0f), vec3(x, 0.0f, half_size_h), blue);
            DDRAW_LINE(vec3(x, 0.0f, -half_size_h), vec3(x, 0.0f, 0.0f), gray);
        }
    }

    /* Row lines and axis */
    for (int j = 1; j < rows; ++j) {
        z = -half_size_h + j * step;

        if (j != half_rows) {
            DDRAW_LINE(vec3(-half_size_w, 0.0f, z), vec3(half_size_w, 0.0f, z), gray);
        } else {
            /* X-axis. */
            DDRAW_LINE(vec3(0.0f, 0.0f, z), vec3(half_size_w, 0.0f, z), red);
            DDRAW_LINE(vec3(-half_size_w, 0.0f, z), vec3(0.0f, 0.0f, z), gray);
        }
    }

    /* Y-axis */
    DDRAW_LINE(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, half_size_w, 0.0f), green);

    /* Lines around grid. */
    DDRAW_LINE(vec3(-half_size_w, 0.0f, half_size_h), vec3(half_size_w, 0.0f, half_size_h), white);
    DDRAW_LINE(vec3(-half_size_w, 0.0f, -half_size_h), vec3(half_size_w, 0.0f, -half_size_h), white);

    DDRAW_LINE(vec3(-half_size_w, 0.0f, half_size_h), vec3(-half_size_w, 0.0f, -half_size_h), white);
    DDRAW_LINE(vec3(half_size_w, 0.0f, half_size_h), vec3(half_size_w, 0.0f, -half_size_h), white);
}

void ddraw_xzgrid(float mins, float maxs, float y, float step) {
    DDRAW_INIT( 2 * DDRAW_CALC(mins,maxs,step) );
    vec3 from, to;
    for( float i = mins; i <= maxs; i += step ) {
        // Horizontal line (along the X)
        from = vec3(mins, y, i);
        to = vec3(maxs, y, i);
        DDRAW_LINE(from, to, gray);

        // Vertical line (along the Z)
        from = vec3(i, y, mins);
        to = vec3(i, y, maxs);
        DDRAW_LINE(from, to, gray);
    }
}

void ddraw_normal(vec3 pos, vec3 dir, float length) {
    DDRAW_INIT(1);
    vec3 dst = {pos.x+dir.x*length, pos.y+dir.y*length, pos.z+dir.z*length};
    DDRAW_LINE(pos, dst, white);
}

void ddraw_axis(vec3 center, float radius) {
    // Red line: X to X + radius
    // Green line: Y to Y + radius
    // Blue line: Z to Z + radius
    DDRAW_INIT(3);
    DDRAW_LINE(center, add3(center, vec3(radius,0,0)), red);
    DDRAW_LINE(center, add3(center, vec3(0,radius,0)), green);
    DDRAW_LINE(center, add3(center, vec3(0,0,radius)), blue);
}

void ddraw_cross(vec3 center, float radius) {
    DDRAW_INIT(6);
    vec3 pos, neg;

    const float cx = center.x;
    const float cy = center.y;
    const float cz = center.z;
    const float hl = radius * 0.5f; // Half on each side.

    // Cyan/Red line: X - radius/2 to X + radius/2
    pos = vec3(cx + hl, cy, cz);
    neg = vec3(cx - hl, cy, cz);
    DDRAW_LINE(center, pos, red);
    DDRAW_LINE(center, neg, cyan);

    // Magenta/Green line: Y - radius/2 to Y + radius/2
    pos = vec3(cx, cy + hl, cz);
    neg = vec3(cx, cy - hl, cz);
    DDRAW_LINE(center, pos, green);
    DDRAW_LINE(center, neg, magenta);

    // Yellow/Blue line: Z - radius/2 to Z + radius/2
    pos = vec3(cx, cy, cz + hl);
    neg = vec3(cx, cy, cz - hl);
    DDRAW_LINE(center, pos, blue);
    DDRAW_LINE(center, neg, yellow);
}



void ddraw_sphere(vec3 center, float radius) {
    const int stepSize = 15;
    vec3 cache[360 / 15/*stepSize*/];
    vec3 radiusVec;

    DDRAW_INIT( 2 * DDRAW_CALC(stepSize,360,stepSize) * DDRAW_CALC(stepSize, 360, stepSize) );

    radiusVec = vec3(0.0f, 0.0f, radius);

    cache[0] = add3(center, radiusVec);
    for (int n = 1; n < sizeof(cache)/sizeof(cache[0]); ++n) {
        cache[n] = cache[0];
    }

    vec3 lastPoint, temp;
    for (int i = stepSize; i <= 360; i += stepSize) {
        const float s = sinf(rad(i));
        const float c = cosf(rad(i));

        lastPoint.x = center.x;
        lastPoint.y = center.y + radius * s;
        lastPoint.z = center.z + radius * c;

        for (int n = 0, j = stepSize; j <= 360; j += stepSize, ++n) {
            temp.x = center.x + sinf(rad(j)) * radius * s;
            temp.y = center.y + cosf(rad(j)) * radius * s;
            temp.z = lastPoint.z;

            DDRAW_LINE(lastPoint, temp, magenta);
            DDRAW_LINE(lastPoint, cache[n], magenta);

            cache[n] = lastPoint;
            lastPoint = temp;
        }
    }

}

static
void ddraw_box_(vec3 points[8]) {
    DDRAW_INIT( 3 * 4 );
    // Build the lines from points using clever indexing tricks:
    // (& 3 is a fancy way of doing % 4, but avoids the expensive modulo operation)
    for( int i = 0; i < 4; ++i ) {
        DDRAW_LINE(points[i], points[(i + 1) & 3], cyan);
        DDRAW_LINE(points[4 + i], points[4 + ((i + 1) & 3)], cyan);
        DDRAW_LINE(points[i], points[4 + i], cyan);
    }
}

void ddraw_box(vec3 center, float width, float height, float depth ) {
    const float cx = center.x;
    const float cy = center.y;
    const float cz = center.z;
    const float w  = width  * 0.5f;
    const float h  = height * 0.5f;
    const float d  = depth  * 0.5f;

    // Create all the 8 points:
    vec3 points[8];
    #define DDRAW_BOX_POINT(v, op1, op2, op3) \
        { v.x = cx op1 w; v.y = cy op2 h; v.z = cz op3 d; }
    DDRAW_BOX_POINT(points[0], -, +, +);
    DDRAW_BOX_POINT(points[1], -, +, -);
    DDRAW_BOX_POINT(points[2], +, +, -);
    DDRAW_BOX_POINT(points[3], +, +, +);
    DDRAW_BOX_POINT(points[4], -, -, +);
    DDRAW_BOX_POINT(points[5], -, -, -);
    DDRAW_BOX_POINT(points[6], +, -, -);
    DDRAW_BOX_POINT(points[7], +, -, +);
    #undef DDRAW_BOX_POINT

    ddraw_box_(points);
}

void ddraw_aabb(vec3 minbb, vec3 maxbb) {
    vec3 bb[2];
    vec3 points[8];

    bb[0] = minbb;
    bb[1] = maxbb;

    // Expand min/max bounds:
    for (int i = 0; i < 8; ++i) {
        points[i].x = bb[(i ^ (i >> 1)) & 1].x;
        points[i].y = bb[     (i >> 1)  & 1].y;
        points[i].z = bb[     (i >> 2)  & 1].z;
    }

    ddraw_box_(points);
}

void ddraw_circle(vec3 center, vec3 normal, float radius) {
    const float segments = 32;

    vec3 left = {0}, up = {0};
    basis3(&left, &up, normal);

    vec3 point, lastPoint;
    up = scale3(up, radius);
    left = scale3(left, radius);
    lastPoint = add3(center, up);

    for (int i = 1; i <= segments; ++i) {
        const float radians = (M__PI * 2) * i / segments;

        vec3 vs = scale3(left, sinf(radians));
        vec3 vc = scale3(up,   cosf(radians));

        point = add3(center, vs);
        point = add3(point,  vc);

        ddraw_line(lastPoint, point);
        lastPoint = point;
    }
}

void ddraw_ring(vec3 center, vec3 normal, float radius) {
    ddraw_circle( center, normal, radius );
    ddraw_circle( center, normal, radius * 0.75f );
}


void ddraw_cone(vec3 begin, vec3 end, float radius) {
    const float bars = 12;

    vec3 x = {0}, y = {0}, n = norm3(sub3(end, begin));
    basis3(&x, &y, n);

    ddraw_circle(end, n, radius);
    for (int i=0; i<bars; ++i) {
        float angle = (i / bars) * 2 * M__PI;
        float c = cosf(angle), s = sinf(angle);
        vec3 end2 = add3( end, add3(scale3(x,radius*c), scale3(y,radius*s)) );
        ddraw_line(begin, end2);
    }
}

void ddraw_arrow(vec3 begin, vec3 end) {
    vec3 diff = sub3(end, begin);
    float len = len3(diff);

    const float head_height = len / 3, head_radius = len / 6;

    vec3 stick = add3(begin, scale3(norm3(diff), head_height*2));

    ddraw_line(begin, stick);
    ddraw_cone(end, stick, head_radius);
}

void ddraw_sphere2(vec3 center, float radius) {
    ddraw_circle(center, vec3(1,0,0), radius);
    ddraw_circle(center, vec3(0,1,0), radius);
    ddraw_circle(center, vec3(0,0,1), radius);
}

void ddraw_frustum(float projview[16]) {
    mat44 clipmatrix = {0}; // clip matrix
    invert44(clipmatrix, projview);

    // Start with the standard clip volume, then bring it back to world space.
    static const vec3 planes[8] = {
        // near plane
        { -1.0f, -1.0f, -1.0f }, {  1.0f, -1.0f, -1.0f },
        {  1.0f,  1.0f, -1.0f }, { -1.0f,  1.0f, -1.0f },
        // far plane
        { -1.0f, -1.0f,  1.0f }, {  1.0f, -1.0f,  1.0f },
        {  1.0f,  1.0f,  1.0f }, { -1.0f,  1.0f,  1.0f }
    };

    vec3 points[8];
    float wCoords[8];

    // Transform the planes by the inverse clip matrix:
    for( int i = 0; i < 8; ++i ) {
        wCoords[i] = matTransformPointXYZW2(&points[i], planes[i], clipmatrix);
        // bail if any W ended up as zero.
        const float epsilon = 1e-9f;
        if (absf(wCoords[i]) < epsilon) {
            return;
        }
    }

    // Divide by the W component of each:
    for( int i = 0; i < 8; ++i ) {
        points[i].x /= wCoords[i];
        points[i].y /= wCoords[i];
        points[i].z /= wCoords[i];
    }

    // Connect the dots:
    ddraw_box_(points);
}

// ----------------------------------------------------------------------------

#include <stdlib.h>
#include <string.h>

static char *ddraw__console[16] = {0};
static int ddraw__line = 0;
void ddraw_console(const char *fmt, ...) {
    //fmt = FORMAT(fmt);
    char buf[256]; va_list vl; va_start(vl, fmt); vsnprintf(buf, 256, fmt, vl); va_end(vl); fmt = buf;

    ddraw__line++; ddraw__line %= 16;
    //strcpyf( &ddraw__console[ddraw__line], "%s", fmt );
    ddraw__console[ddraw__line] = realloc( ddraw__console[ddraw__line], strlen(fmt)+1); strcpy( ddraw__console[ddraw__line], fmt );
}

// ----------------------------------------------------------------------------

static renderable_t shapes[1024] = {0};
static int instanced_shapes = 0;

static
renderable_t *ddraw_find_slot() {
    // find dead slot
    if( instanced_shapes < 1024 ) {
        return &shapes[instanced_shapes++];
    }
    // no luck
    return 0;
}

static
void ddraw_clear() {
    for( int i = 0; i < instanced_shapes; ++i ) {
        // bool is_defunct = shapes[i].age && shapes[i].tick > shapes[i].age;
        // bool is_volatile = !shapes[i].age;
        renderable_destroy(&shapes[i]);
    }
    memset( shapes, 0, sizeof(renderable_t) * 1024 );
    instanced_shapes = 0;
}

// default embedded font
//const
int ddraw_font = 0;

void ddraw_text2d( vec2 center, const char *fmt, ... ) {
    renderable_t *r = ddraw_find_slot();
    if( !r ) return;

    //fmt = FORMAT(fmt);
    char buf[256]; va_list vl; va_start(vl, fmt); vsnprintf(buf, 256, fmt, vl); va_end(vl); fmt = buf;


#if 0
    if(!ddraw_font && __argc > 1) {
        ddraw_font = font(__argv[1], __argc > 2 ? atoi(__argv[2]) : 8, FONT_RANGE_EU /*512, no oversample*/ );
    }
#endif

    // create mesh
    text( r, ddraw_font, fmt );

    // @todo: fix this
    // create transform
    float scale = 3;
    identity44(r->tf.matrix);
#if 1
    scale44(r->tf.matrix, scale / window_get(WINDOW_WIDTH)[0], scale / window_get(WINDOW_HEIGHT)[0], 1);
    translate44( r->tf.matrix, center.x, center.y, 0 );
#endif
}

// ----------------------------------------------------------------------------

enum { MAX_MENUS = 256 };

int ddraw_menu( int submenu_id, const char **options ) {
    if( submenu_id < 0 || submenu_id >= MAX_MENUS ) {
        return -1;
    }
    if( !options ) {
        return -1;
    }

    int options_size = 0;
    for( int i = 0; options[i]; ++i ) {
        ++options_size;
    }

    if( !options_size ) {
        return -1;
    }

    static int selection[MAX_MENUS] = {0};

    int *s = &selection[ 0 ];
    if( key('UP') ) { *s = *s - 1; if( *s < 0 ) *s = options_size - 1; }
    if( key('DOWN') ) { *s = *s + 1; if( *s >= options_size ) *s = 0; }

    for( int i = 0; options[i]; ++i ) {
        vec2 pos = {0, -i * 12 };
        ddraw_text2d( pos, "%s%s", *s == i ? ">> " : " - ", options[i] );
    }

    return key('\n') ? *s : -1;
}

// ----------------------------------------------------------------------------

void ddraw_render2d( float *pm, float *vm ) {
    // texts
    mat44 proj;
    ortho(proj, ORTHO_CENTERED | ORTHO_NORMALIZED );
    renderer_enable(&fonts[ddraw_font].r, proj);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE); // additive: 1 * src_color + 1 * fb_color
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // decal: src_color.a * src_color + (1 - src_color.a) * fb_color
    //glBlendFunc(GL_DST_COLOR, GL_ZERO); // modulate: fb_color * src_color + 0 * fb_color

    for( int i = 0; i < instanced_shapes; ++i ) {
        draw(&shapes[i], shapes[i].tf.matrix);
    }

    // update console
    ortho(proj, ORTHO_TOPLEFT | ORTHO_NORMALIZED);
    renderer_enable(&fonts[ddraw_font].r, proj);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE); // additive: 1 * src_color + 1 * fb_color
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // decal: src_color.a * src_color + (1 - src_color.a) * fb_color
    //glBlendFunc(GL_DST_COLOR, GL_ZERO); // modulate: fb_color * src_color + 0 * fb_color

    float spacing = fonts[ddraw_font].spaceY;
    for(int l = ddraw__line; l < ddraw__line + 16; ++l) {
        int p = l - ddraw__line;
        int i = l % 16;
        if( ddraw__console[i] ) {
            float scale = 3;

            mat44 m;
            identity44(m);
            scale44(m, scale / window_get(WINDOW_WIDTH)[0], scale / window_get(WINDOW_HEIGHT)[0], 1);
            translate44( m, 0, -p * spacing,0 );
            renderable_t r = {0};
            text(&r, ddraw_font, ddraw__console[i] );
            draw(&r, m);
            renderable_destroy(&r);
        }
    }

    ddraw_clear();
}

#endif

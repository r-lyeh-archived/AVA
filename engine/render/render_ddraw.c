// based on code by @glampert (public domain) and @vurtun (public domain).
// [src] https://gist.github.com/vurtun/95f088e4889da2474ad1ce82d7911fee
// - rlyeh, public domain.
//
// @todo: split into 2d and 3d? dd2_ dd3_ prefixes?

#ifndef DDRAW_H
#define DDRAW_H

// global state

API void ddraw_color4(vec4 rgba);
API void ddraw_color3(vec3 rgb);
//API void ddraw_enable(bool on);
//API void ddraw_push( float scale, float expire_ss, bool depth_on );
//API void ddraw_pop();

// 2d

API void ddraw_printf( const char *buf );
API void ddraw_console( const char *buf );
API void ddraw_text2d( vec2 center, const char *buf );

// 3d

API void ddraw_begin(float projview[16]);
API void ddraw_aabb(vec3 minbb, vec3 maxbb);
API void ddraw_arrow(vec3 from, vec3 to);
API void ddraw_axis(vec3 center, float radius);
API void ddraw_bounds(const vec3 points[8]);
API void ddraw_box(vec3 center, vec3 extents);
API void ddraw_capsule(vec3 from, vec3 to, float r);
API void ddraw_circle(vec3 center, vec3 dir, float radius);
API void ddraw_cone(vec3 pos1, vec3 pos2, float radius);
API void ddraw_cross(vec3 center, float radius);
API void ddraw_diamond(vec3 from, vec3 to, float size);
API void ddraw_frustum(float projview_matrix[16]);
API void ddraw_grid(int hcols, int hrows, float step);
API void ddraw_line(vec3 from, vec3 to);
API void ddraw_line2(vec3 src, vec3 dst, vec4 col); // src_col, dst_col
API void ddraw_normal(vec3 center, vec3 dir, float radius);
API void ddraw_plane(vec3 p, vec3 n, float scale);
API void ddraw_pyramid(vec3 f, vec3 t, float size);
API void ddraw_quad(vec3 a, vec3 b, vec3 c, vec3 d);
API void ddraw_ring(vec3 center, vec3 dir, float radius);
API void ddraw_sphere(vec3 center, float radius);
API void ddraw_sphere2(vec3 center, float radius);
API void ddraw_triangle(vec3 p, vec3 q, vec3 r);
//API void ddraw_rect(float a[2], float b[2]);
//API void ddraw_text3d( vec3 center, const char *fmt, ... );
//API void ddraw_point(vec3 location);
API void ddraw_end(void);

// --

#define ddraw_printf(...)   ddraw_printf(va(__VA_ARGS__))
#define ddraw_console(...)   ddraw_console(va(__VA_ARGS__))
#define ddraw_text2d(p,...) ddraw_text2d(p,va(__VA_ARGS__))

#endif

// ----------------------------------------------------------------------------

#ifdef DDRAW_C
#pragma once
#include "render_opengl.c"
#include "render_font.c"
#include "engine.h" // app/window

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
    vec2 rect = window_size();
    float w = rect.x, h = rect.y + 1;
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
            ortho44(proj, 0,w,-h,0, -1,1 );
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
    "in vec3 att_position;\n"
    "in vec4 att_color;\n"
    "/* transform vertex and transfer color */\n"
    "out vec4 var_color;\n"
    "void main() {\n"
    "   gl_Position = projview * vec4(att_position.xyz, 1.0);\n"
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

/*
 * Line and rect drawing.
 */

#define DD_MAXBUFFER (1024 * 1024)

static unsigned int draw_vao = 0;
static unsigned int draw_vbo = 0;
static int draw_buf_len = 0;
static struct vertex_p3c4_ { vec3 position; vec4 color; } *draw_buf = 0; // [DD_MAXBUFFER];
static int sizeof_drawbuf = sizeof(struct vertex_p3c4_) * DD_MAXBUFFER;

static vec4 draw_color = { 1, 1, 1, 1 };
static int draw_kind = GL_LINES;

void ddraw_begin(float projview[16]) {
    static int draw_prog = 0;
    static int draw_uni_projview = -1;

    if( !draw_buf ) {
        draw_buf = (struct vertex_p3c4_ *)MALLOC( sizeof_drawbuf );
    }

    if (!draw_prog) {
        draw_prog = shader2(ddraw_shader_vtxpc_vs, ddraw_shader_vtxpc_fs, "att_position,att_color");
        draw_uni_projview = glGetUniformLocation(draw_prog, "projview");
    }

    if (!draw_vao) {
        glGenVertexArrays(1, &draw_vao);
        glBindVertexArray(draw_vao);

        glGenBuffers(1, &draw_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, draw_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof_drawbuf, NULL, GL_STREAM_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, 0, sizeof(struct vertex_p3c4_), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, 0, sizeof(struct vertex_p3c4_), (void*)12);

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

/*
void ddraw_triangle_(vec3 a, vec3 b, vec3 c) {
    ddraw_line(a,b);
    ddraw_line(b,c);
    ddraw_line(c,a);
}
*/

void ddraw_triangle(vec3 p, vec3 q, vec3 r) {
    if( draw_kind != GL_TRIANGLES || draw_buf_len + 3 >= DD_MAXBUFFER ) {
        ddraw_flush();
    }

    draw_kind = GL_TRIANGLES;
    ddraw_vertex(p);
    ddraw_vertex(q);
    ddraw_vertex(r);
}

void ddraw_quad(vec3 a, vec3 b, vec3 c, vec3 d) {
    if( draw_kind != GL_TRIANGLES || draw_buf_len + 6 >= DD_MAXBUFFER ) {
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
    if( draw_kind != GL_TRIANGLES || draw_buf_len + 6 >= DD_MAXBUFFER ) {
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

#define DDRAW_LINES(NUM_LINES) do { \
    int num_vertices = NUM_LINES * 2; \
    if( draw_kind != GL_LINES || (draw_buf_len + num_vertices >= DD_MAXBUFFER)) { \
        ddraw_flush(); \
    } \
} while(0)

static
void DDRAW_LINE4( vec3 src, vec3 dst, vec4 color ) {
    ddraw_vertexEx( src, color );
    ddraw_vertexEx( dst, color );
}

static
void DDRAW_LINE( vec3 src, vec3 dst ) {
    DDRAW_LINE4( src, dst, draw_color );
}

void ddraw_line(vec3 p, vec3 q) {
    if( draw_kind != GL_LINES || draw_buf_len + 2 >= DD_MAXBUFFER ) {
        ddraw_flush();
    }

    draw_kind = GL_LINES;
    ddraw_vertex(p);
    ddraw_vertex(q);
}

void ddraw_line2( vec3 src, vec3 dst, vec4 col ) {
    DDRAW_LINES(1);
    DDRAW_LINE4(src, dst, col);
}

/*
void ddraw_grid_(float mins, float maxs, float y, float step) {
    for( float i = mins; i <= maxs; i += step ) {
        ddraw_line(vec3(mins,y,i), vec3(maxs,y,i));
        ddraw_line(vec3(i,y,mins), vec3(i,y,maxs));
    }
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

    DDRAW_LINES( (cols+rows)*2 + 5 );

    /* Column lines and axis. */
    for (int i = 1; i < cols; ++i) {
        x = -half_size_w + i * step;

        if (i != half_cols) {
            DDRAW_LINE4(vec3(x, 0.0f, -half_size_h), vec3(x, 0.0f, half_size_h), gray);
        } else {
            /* Z-axis. */
            DDRAW_LINE4(vec3(x, 0.0f, 0.0f), vec3(x, 0.0f, half_size_h), blue);
            DDRAW_LINE4(vec3(x, 0.0f, -half_size_h), vec3(x, 0.0f, 0.0f), gray);
        }
    }

    /* Row lines and axis */
    for (int j = 1; j < rows; ++j) {
        z = -half_size_h + j * step;

        if (j != half_rows) {
            DDRAW_LINE4(vec3(-half_size_w, 0.0f, z), vec3(half_size_w, 0.0f, z), gray);
        } else {
            /* X-axis. */
            DDRAW_LINE4(vec3(0.0f, 0.0f, z), vec3(half_size_w, 0.0f, z), red);
            DDRAW_LINE4(vec3(-half_size_w, 0.0f, z), vec3(0.0f, 0.0f, z), gray);
        }
    }

    /* Y-axis */
    DDRAW_LINE4(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, half_size_w, 0.0f), green);

    /* Lines around grid. */
    DDRAW_LINE4(vec3(-half_size_w, 0.0f, half_size_h), vec3(half_size_w, 0.0f, half_size_h), white);
    DDRAW_LINE4(vec3(-half_size_w, 0.0f, -half_size_h), vec3(half_size_w, 0.0f, -half_size_h), white);

    DDRAW_LINE4(vec3(-half_size_w, 0.0f, half_size_h), vec3(-half_size_w, 0.0f, -half_size_h), white);
    DDRAW_LINE4(vec3(half_size_w, 0.0f, half_size_h), vec3(half_size_w, 0.0f, -half_size_h), white);
}

void ddraw_normal(vec3 pos, vec3 dir, float length) {
    DDRAW_LINES(1);
    DDRAW_LINE4(pos, add3(pos,scale3(dir,length)), white);
}

void ddraw_axis(vec3 center, float radius) {
    DDRAW_LINES(3);
    DDRAW_LINE4(center, add3(center, vec3(radius,0,0)), red);
    DDRAW_LINE4(center, add3(center, vec3(0,radius,0)), green);
    DDRAW_LINE4(center, add3(center, vec3(0,0,radius)), blue);
}

void ddraw_cross(vec3 center, float radius) {
    DDRAW_LINES(6);

    float hl = radius * 0.5f; // Half on each side.
    float cx = center.x, cy = center.y, cz = center.z;

    // Cyan/Red line: X - radius/2 to X + radius/2
    vec3 pos = vec3(cx + hl, cy, cz);
    vec3 neg = vec3(cx - hl, cy, cz);
    DDRAW_LINE4(center, pos, red);
    DDRAW_LINE4(center, neg, cyan);

    // Magenta/Green line: Y - radius/2 to Y + radius/2
    pos = vec3(cx, cy + hl, cz);
    neg = vec3(cx, cy - hl, cz);
    DDRAW_LINE4(center, pos, green);
    DDRAW_LINE4(center, neg, magenta);

    // Yellow/Blue line: Z - radius/2 to Z + radius/2
    pos = vec3(cx, cy, cz + hl);
    neg = vec3(cx, cy, cz - hl);
    DDRAW_LINE4(center, pos, blue);
    DDRAW_LINE4(center, neg, yellow);
}

void ddraw_circle(vec3 center, vec3 normal, float radius) {
    vec3 left = {0}, up = {0};
    ortho3(&left, &up, normal);

    vec3 point, lastPoint;
    up = scale3(up, radius);
    left = scale3(left, radius);
    lastPoint = add3(center, up);

    enum { segments = 32 };
    for (int i = 1; i <= segments; ++i) {
        const float radians = (C_PI * 2) * i / segments;

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
    vec3 x = {0}, y = {0}, n = norm3(sub3(end, begin));
    ortho3(&x, &y, n);

    enum { bars = 12 };
    ddraw_circle(end, n, radius);
    for (int i = 0; i < bars; ++i) {
        float angle = ((float)i / bars) * 2 * C_PI;
        float c = cosf(angle), s = sinf(angle);
        vec3 end2 = add3( end, add3(scale3(x,radius*c), scale3(y,radius*s)) );
        ddraw_line(begin, end2);
    }
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
    const vec3 planes[8] = {
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
    ddraw_bounds(points);
}

// ----------------------------------------------------------------------------

void ddraw_color4(vec4 c) {
    draw_color = c;
}

void ddraw_color3(vec3 rgb) {
    ddraw_color4(vec34(rgb,1));
}

void ddraw_plane(vec3 p, vec3 n, float scale) {
    vec3 tangent, bitangent;
    ortho3(&tangent, &bitangent, n);

    vec3 v1, v2, v3, v4;
    #define DD_PLANE_V(v, op1, op2) \
        v.x = (p.x op1 (tangent.x*scale) op2 (bitangent.x*scale)); \
        v.y = (p.y op1 (tangent.y*scale) op2 (bitangent.y*scale)); \
        v.z = (p.z op1 (tangent.z*scale) op2 (bitangent.z*scale))
    DD_PLANE_V(v1,-,-);
    DD_PLANE_V(v2,+,-);
    DD_PLANE_V(v3,+,+);
    DD_PLANE_V(v4,-,+);
    #undef DD_PLANE_V

    ddraw_line(v1,v2);
    ddraw_line(v2,v3);
    ddraw_line(v3,v4);
    ddraw_line(v4,v1);
}

void ddraw_capsule(vec3 from, vec3 to, float r) {
    /* calculate axis */
    vec3 up, right, forward;
    forward = sub3(to, from);
    forward = norm3(forward);
    ortho3(&right, &up, forward);

    /* calculate first two cone verts (buttom + top) */
    vec3 lastf, lastt;
    lastf = scale3(up,r);
    lastt = add3(to,lastf);
    lastf = add3(from,lastf);

    /* step along circle outline and draw lines */
    enum { step_size = 20 };
    for (int i = step_size; i <= 360; i += step_size) {
        /* calculate current rotation */
        vec3 ax = scale3(right, sinf(i*TO_RAD));
        vec3 ay = scale3(up, cosf(i*TO_RAD));

        /* calculate current vertices on cone */
        vec3 tmp = add3(ax, ay);
        vec3 pf = scale3(tmp, r);
        vec3 pt = scale3(tmp, r);

        pf = add3(pf, from);
        pt = add3(pt, to);

        /* draw cone vertices */
        ddraw_line(lastf, pf);
        ddraw_line(lastt, pt);
        ddraw_line(pf, pt);

        lastf = pf;
        lastt = pt;

        /* calculate first top sphere vert */
        vec3 prevt = scale3(tmp, r);
        vec3 prevf = add3(prevt, from);
        prevt = add3(prevt, to);

        /* sphere (two half spheres )*/
        for (int j = 1; j < 180/step_size; j++) {
            /* angles */
            float ta = j*step_size;
            float fa = 360-(j*step_size);

            /* top half-sphere */
            ax = scale3(forward, sinf(ta*TO_RAD));
            ay = scale3(tmp, cosf(ta*TO_RAD));

            vec3 t = add3(ax, ay);
            pf = scale3(t, r);
            pf = add3(pf, to);
            ddraw_line(pf, prevt);
            prevt = pf;

            /* bottom half-sphere */
            ax = scale3(forward, sinf(fa*TO_RAD));
            ay = scale3(tmp, cosf(fa*TO_RAD));

            t = add3(ax, ay);
            pf = scale3(t, r);
            pf = add3(pf, from);
            ddraw_line(pf, prevf);
            prevf = pf;
        }
    }
}

void ddraw_pyramid(vec3 f, vec3 t, float size) {
    poly p = pyramid(f, t, size);
    vec3 *pyr = p.verts;

    vec3 *a = pyr + 0;
    vec3 *b = pyr + 1;
    vec3 *c = pyr + 2;
    vec3 *d = pyr + 3;
    vec3 *r = pyr + 4;

    /* draw vertices */
    ddraw_line(*a, *b);
    ddraw_line(*b, *c);
    ddraw_line(*c, *d);
    ddraw_line(*d, *a);

    /* draw root */
    ddraw_line(*a, *r);
    ddraw_line(*b, *r);
    ddraw_line(*c, *r);
    ddraw_line(*d, *r);

    array_free(pyr);
}

void ddraw_diamond(vec3 from, vec3 to, float size) {
    poly p = diamond(from, to, size);
    vec3 *dmd = p.verts;

    vec3 *a = dmd + 0;
    vec3 *b = dmd + 1;
    vec3 *c = dmd + 2;
    vec3 *d = dmd + 3;
    vec3 *t = dmd + 4;
    vec3 *f = dmd + 5;

    /* draw vertices */
    ddraw_line(*a, *b);
    ddraw_line(*b, *c);
    ddraw_line(*c, *d);
    ddraw_line(*d, *a);

    /* draw roof */
    ddraw_line(*a, *t);
    ddraw_line(*b, *t);
    ddraw_line(*c, *t);
    ddraw_line(*d, *t);

    /* draw floor */
    ddraw_line(*a, *f);
    ddraw_line(*b, *f);
    ddraw_line(*c, *f);
    ddraw_line(*d, *f);

    array_free(dmd);
}

void ddraw_aabb(vec3 minbb, vec3 maxbb) {
    vec3 points[8], bb[2] = { minbb, maxbb };
    for (int i = 0; i < 8; ++i) {
        points[i].x = bb[(i ^ (i >> 1)) & 1].x;
        points[i].y = bb[     (i >> 1)  & 1].y;
        points[i].z = bb[     (i >> 2)  & 1].z;
    }
    ddraw_bounds(points);
}

void ddraw_bounds(const vec3 points[8]) {
    DDRAW_LINES( 4 * 3 );
    for( int i = 0; i < 4; ++i ) {
        DDRAW_LINE(points[i], points[(i + 1) & 3]);
        DDRAW_LINE(points[i], points[4 + i]);
        DDRAW_LINE(points[4 + i], points[4 + ((i + 1) & 3)]);
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

void ddraw_sphere(vec3 center, float radius) {
    enum { STEP = 15 };
    vec3 cache[360 / STEP], last, tmp;

    DDRAW_LINES( 2 * ((360+1-STEP)/STEP) * ((360+1-STEP)/STEP) );

    cache[0] = add3(center, vec3(0,0,radius));
    for (int n = 1; n < countof(cache); ++n) {
        cache[n] = cache[0];
    }

    /* 1st circle iteration */
    for (int i = STEP; i <= 360; i += STEP) {
        float r = rad(i), s = sinf(r), c = cosf(r);
        last = add3(center, vec3(0,radius*s,radius*c));

        /* 2nd circle iteration */
        for (int j = STEP, n = 0; j <= 360; j += STEP, ++n) {
            tmp.x = center.x + sinf(rad(j)) * radius * s;
            tmp.y = center.y + cosf(rad(j)) * radius * s;
            tmp.z = last.z;

            DDRAW_LINE(last, tmp);
            DDRAW_LINE(last, cache[n]);

            cache[n] = last;
            last = tmp;
        }
    }
}

void ddraw_box(vec3 c, vec3 extents) {
    vec3 points[8], whd = scale3(extents, 0.5f);
    #define DD_BOX_V(v, op1, op2, op3) (v).x = c.x op1 whd.x; (v).y = c.y op2 whd.y; (v).z = c.z op3 whd.z
    DD_BOX_V(points[0], -, +, +);
    DD_BOX_V(points[1], -, +, -);
    DD_BOX_V(points[2], +, +, -);
    DD_BOX_V(points[3], +, +, +);
    DD_BOX_V(points[4], -, -, +);
    DD_BOX_V(points[5], -, -, -);
    DD_BOX_V(points[6], +, -, -);
    DD_BOX_V(points[7], +, -, +);
    #undef DD_BOX_V
    ddraw_bounds(points);
}

// ----------------------------------------------------------------------------

#include <stdlib.h>
#include <string.h>

static int console_line_ = 0;
static char *console_lines_[16] = {0};

void (ddraw_console)(const char *buf) {
    console_line_++; console_line_ %= 16;
    console_lines_[console_line_] = realloc( console_lines_[console_line_], strlen(buf)+1); 
    strcpy( console_lines_[console_line_], buf );
}

// ----------------------------------------------------------------------------

static mesh shapes[1024] = {0};
static mat44 matrices[1024] = {0};
static int instanced_shapes = 0;

static
int ddraw_find_slot() {
    // find dead slot
    if( instanced_shapes < 1024 ) {
        return instanced_shapes++;
    }
    // no luck
    return -1;
}

static
void ddraw_clear() {
    for( int i = 0; i < instanced_shapes; ++i ) {
        // bool is_defunct = shapes[i].age && shapes[i].tick > shapes[i].age;
        // bool is_volatile = !shapes[i].age;
        mesh_destroy(&shapes[i]);
    }
    memset( shapes, 0, sizeof(mesh) * 1024 );
    instanced_shapes = 0;
}

// default embedded font

int ddraw_font = 0;
int ddraw_spacey = 10; // fonts[0].spaceY

void (ddraw_text2d)( vec2 pos, const char *buf ) {
    int slot = ddraw_find_slot();
    if( slot < 0 ) return;

    mesh *r = &shapes[slot];
    float* tf = matrices[slot];

    // create mesh
    font_mesh(r, ddraw_font, buf);
    scaling44(tf, 1,1,1);
    translate44(tf, pos.x, - pos.y, 0 );
}

//-----------------------------------------------------------------------------

static int ddraw_printf_line = 0;

void (ddraw_printf)(const char *buf) {
    (ddraw_text2d)( vec2(3,++ddraw_printf_line * ddraw_spacey), buf );
}

// ----------------------------------------------------------------------------

static
void ddraw_render2d() {
    // setup material
    static material mat, *init = 0;
    if( !init ) {
        mat = *font_material();
        mat.texture = fonts[ddraw_font].texture_id;
        mat.depth_func = GL_LEQUAL;
        mat.alpha_enable = 1;
        mat.alpha_src = GL_ONE, mat.alpha_dst = GL_ONE_MINUS_SRC_ALPHA; // premultiplied
        mat.alpha_src = GL_DST_COLOR, mat.alpha_dst = GL_ZERO; // modulate
        mat.alpha_src = GL_ONE, mat.alpha_dst = GL_ONE; // additive
        mat.alpha_src = GL_SRC_ALPHA, mat.alpha_dst = GL_ONE_MINUS_SRC_ALPHA; // decal
    }

    // setup matrices
    mat44 view;
    identity44(view);

    mat44 proj2d;
    ortho(proj2d, ORTHO_CENTERED | ORTHO_NORMALIZED );

    mat44 projview2d_center;
    multiply44(projview2d_center, view, proj2d);

    mat44 projview2d_topleft;
    ortho(proj2d, ORTHO_TOPLEFT /*| ORTHO_NORMALIZED */ );
    multiply44(projview2d_topleft, view, proj2d);

    mat44 projview2d_bottomleft;
    ortho(proj2d, ORTHO_BOTTOMLEFT /*| ORTHO_NORMALIZED */ );
    multiply44(projview2d_bottomleft, view, proj2d);

    // texts
    material_enable(&mat, projview2d_topleft);
    for( int i = 0; i < instanced_shapes; ++i ) {
        mesh *r = &shapes[i];
        r->material = &mat;
        material_sendmodel(r->material, matrices[i]);
        mesh_render(r, mat.shader);
    }

    // draw console
    material_enable(&mat, projview2d_bottomleft);
    float spacing = fonts[ddraw_font].spaceY;
    for(int l = 0; l < 16; ++l) {
        int p = console_line_ + l;
        int i = (p+1) % 16;
        if( console_lines_[i] ) {
            mat44 m;
            identity44(m);
            translate44(m, 0,(l+1)*spacing,0);

            mesh r = { 0 };
            font_mesh(&r, ddraw_font, console_lines_[i]);
            r.material = &mat;
            material_sendmodel(r.material, m);
            mesh_render(&r, mat.shader);
            mesh_destroy(&r);
        }
    }
    // draw console input
    static unsigned char cursor = 0;
    {
        mat44 m;
        identity44(m);
        translate44( m, 0, 0, 0 );

        mesh r = { 0 };
        font_mesh(&r, ddraw_font, (++cursor & 0x20) ? "> " : ">_" ); // &0x20 =~ every 0.5s at 60fps
        r.material = &mat;
        material_sendmodel(r.material, m);
        mesh_render(&r, mat.shader);
        mesh_destroy(&r);
    }

    ddraw_clear();
}

#endif

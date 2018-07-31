// based on code by @glampert (public domain).
// - rlyeh, public domain.
//
// @todo: split into ddraw_render2d and ddraw_render3d ?

#ifndef DDRAW_H
#define DDRAW_H

#ifndef degrees_
#define degrees_ *(3.14159f/180)
#endif

// add functions
void ddraw_aabb(float minbb[3], float maxbb[3]);
void ddraw_arrow(float from[3], float to[3]);
void ddraw_axis(float center[3], float radius);
void ddraw_box(float center[3], float width, float height, float depth); //void ddraw_box_(vec3 points[8]); // instead?
void ddraw_circle(float center[3], float dir[3], float radius);
void ddraw_cone(float pos1[3], float pos2[3], float radius);
void ddraw_cross(float center[3], float radius);
void ddraw_frustum(float projview_matrix[16]);
void ddraw_grid(int hcols, int hrows, float step);
void ddraw_line(float from[3], float to[3]); //void ddraw_lineEx( float src[3], float dst[3], float srccolor[4], float dstcolor[4] );
void ddraw_normal(float center[3], float dir[3], float radius);
void ddraw_ring(float center[3], float dir[3], float radius);
void ddraw_sphere(float center[3], float radius);
void ddraw_sphere2(float center[3], float radius);
void ddraw_xzgrid(float mins, float maxs, float y, float step);

 int ddraw_menu( int submenu_id, const char **lines );
void ddraw_text2d( float center[2], const char *fmt, ... );
void ddraw_text3d( float center[3], const char *fmt, ... );
void ddraw_console( const char *fmt, ... );
void ddraw_render2d( /*int shader, int camera*/ float *projmatrix, float *viewmatrix );

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

void ddraw_triangle(float p[3], float q[3], float r[3]);
void ddraw_quad(float a[3], float b[3], float c[3], float d[3]);
//void ddraw_rect(float a[2], float b[2]);
//void ddraw_box_(float points[8][3]);

void ddraw_lineEx( float src[3], float dst[3], float col[4] );

#endif

// ----------------------------------------------------------------------------

#ifdef DDRAW_C
#pragma once
#include "render_renderer.c"
#include "render_text.c"
#include "render_font.c"
#include "../framework/window.c"
#include "render_ddraw.c"

// find/compute basis/triad/axes from normal

static void vec3basis(float tangent[3], float bitangent[3], const float normal[3]) {
    // perp_vector(tangent, normal)
    float a = normal[0], b = normal[1], c = normal[2];
    if( c * c < a * a ) {
        vec3set(tangent,b,-a,0);
    } else {
        vec3set(tangent, 0,-c,b);
    }

    vec3_mul_cross(bitangent, tangent, normal);
}

// normalize direction vector

static void vec3normdir(float n[3], float from[3], float to[3] ) {
    // unit = normalize( to - from );
    vec3set(n, from[0] - to[0], from[1] - to[1], from[2] - to[2]);
    vec3_norm(n, n);
}

// 

static float matTransformPointXYZW2(float out[3], const float in[3], const float m[16]) {
    enum { X,Y,Z };
    out[X] = (m[0] * in[X]) + (m[4] * in[Y]) + (m[8]  * in[Z]) + m[12]; // in[W] assumed to be 1
    out[Y] = (m[1] * in[X]) + (m[5] * in[Y]) + (m[9]  * in[Z]) + m[13];
    out[Z] = (m[2] * in[X]) + (m[6] * in[Y]) + (m[10] * in[Z]) + m[14];
    return (m[3] * in[X]) + (m[7] * in[Y]) + (m[11] * in[Z]) + m[15]; // rw
}

// ----------------------------------------------------------------------------

// base colors: RGB, CMY, B/W

static float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
static float green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
static float blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

static float cyan[4] = { 0.0f, 1.0f, 1.f, 1.0f };
static float magenta[4] = { 1.0f, 0.0f, 1.f, 1.0f };
static float yellow[4] = { 1.0f, 1.0f, 0.f, 1.0f };

static float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
static float gray[4] = { 0.3f, 0.3f, 0.3f, 0.6f };
static float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f};

// ----------------------------------------------------------------------------

enum {
    ORTHO_TOPLEFT = 1,
    ORTHO_CENTERED = 2,
    ORTHO_BOTTOMLEFT = 4,
    //
    ORTHO_NORMALIZED = 8,
};

void ortho(mat4x4 proj, int flags) {
    // void mat4x4_ortho(mat4x4 M, float l, float r, float b, float t, float n, float f);
    // @todo: test all configurations
    float w = window_get(WINDOW_WIDTH)[0];
    float h = window_get(WINDOW_HEIGHT)[0] + 1.f;
    if( flags & ORTHO_CENTERED ) {
        if( flags & ORTHO_NORMALIZED ) {
            // (-1,-1) top-left, (+1,+1) bottom-right
            mat4x4_ortho(proj, -1, +1, -1, +1, -1,1 ); 
        } else {
            // (-w/2,-h/2) top-left, (0,0) center, (+w/2,+h/2) bottom-right
            mat4x4_ortho(proj, -w/2,w/2,-h/2,h/2, -1,1 ); 
        }
    }
    if( flags & ORTHO_TOPLEFT ) {
        if( flags & ORTHO_NORMALIZED ) {
            // (0,0) top-left, (+1,+1) bottom-right
            mat4x4_ortho(proj, -1, +1, -1, +1, -1,1 );
            mat4x4_translate_in_place(proj, -1, +1, 0 ); 
        } else {
            // (0,0) top-left, (w,h) bottom-right
            mat4x4_ortho(proj, 0,w,h,0, -1,1 );
        }
    }
    if( flags & ORTHO_BOTTOMLEFT ) {
        if( flags & ORTHO_NORMALIZED ) {
            // (0,0) bottom-left, (+1,+1) top-right
            mat4x4_ortho(proj, -1, +1, +1, -1, -1,1 );
            mat4x4_translate_in_place(proj, -1, -1, 0 ); 
        } else {
            // (0,0) bottom-left, (w,h) top-right
            mat4x4_ortho(proj, 0,w,0,h, -1,1 );
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
static struct vertex_p3c4_ { float position[3], color[4]; } draw_buf[MAXBUFFER];

static float draw_color[4] = { 1, 1, 1, 1 };
static int draw_kind = GL_LINES;

void ddraw_color(float r, float g, float b, float a) {
    draw_color[0] = r;
    draw_color[1] = g;
    draw_color[2] = b;
    draw_color[3] = a;
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

static void ddraw_vertexEx(float pos[3], float draw_color[4]) {
    draw_buf[draw_buf_len].position[0] = pos[0];
    draw_buf[draw_buf_len].position[1] = pos[1];
    draw_buf[draw_buf_len].position[2] = pos[2];
    draw_buf[draw_buf_len].color[0] = draw_color[0];
    draw_buf[draw_buf_len].color[1] = draw_color[1];
    draw_buf[draw_buf_len].color[2] = draw_color[2];
    draw_buf[draw_buf_len].color[3] = draw_color[3];
    draw_buf_len++;
}

static void ddraw_vertex(vec3 pos) {
    ddraw_vertexEx(pos, draw_color);
}

void ddraw_line(float p[3], float q[3]) {
    if( draw_kind != GL_LINES || draw_buf_len + 2 >= MAXBUFFER ) {
        ddraw_flush();
    }

    draw_kind = GL_LINES;
    ddraw_vertex(p);
    ddraw_vertex(q);
}

void ddraw_triangle(float p[3], float q[3], float r[3]) {
    if( draw_kind != GL_TRIANGLES || draw_buf_len + 3 >= MAXBUFFER ) {
        ddraw_flush();
    }

    draw_kind = GL_TRIANGLES;
    ddraw_vertex(p);
    ddraw_vertex(q);
    ddraw_vertex(r);
}

void ddraw_quad(float a[3], float b[3], float c[3], float d[3]) {
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

    enum { x, y };

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

#define INIT(NUM_LINES) do { \
    int num_vertices = NUM_LINES * 2; \
    if( draw_kind != GL_TRIANGLES || (draw_buf_len + num_vertices >= MAXBUFFER)) { \
        ddraw_flush(); \
    } \
} while(0)
#define CALC(min,max,step) ((max + 1 - min) / step)

void LINE( float src[3], float dst[3], float color[4] ) {
    ddraw_vertexEx( src, color );
    ddraw_vertexEx( dst ,color );
}


void ddraw_lineEx( float src[3], float dst[3], float col[4] ) {
    INIT(1);
    LINE(src, dst, col);
}

/*
void ddraw_line( float src[3], float dst[3] ) {
    INIT(1);
    LINE(src, dst, yellow );
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

    INIT( (cols+rows)*2 + 5 );

    /* Column lines and axis. */
    for (int i = 1; i < cols; ++i) {
        x = -half_size_w + i * step;

        if (i != half_cols) {
            LINE((vec3){ x, 0.0f, -half_size_h }, (vec3){ x, 0.0f, half_size_h }, gray);
        } else {
            /* Z-axis. */
            LINE((vec3){ x, 0.0f, 0.0f }, (vec3){ x, 0.0f, half_size_h }, blue);
            LINE((vec3){ x, 0.0f, -half_size_h }, (vec3){ x, 0.0f, 0.0f }, gray);
        }
    }

    /* Row lines and axis */
    for (int j = 1; j < rows; ++j) {
        z = -half_size_h + j * step;

        if (j != half_rows) {
            LINE((vec3){ -half_size_w, 0.0f, z }, (vec3){ half_size_w, 0.0f, z }, gray);
        } else {
            /* X-axis. */
            LINE((vec3){ 0.0f, 0.0f, z }, (vec3){ half_size_w, 0.0f, z }, red);
            LINE((vec3){ -half_size_w, 0.0f, z }, (vec3){ 0.0f, 0.0f, z }, gray);
        }
    }

    /* Y-axis */
    LINE((vec3){ 0.0f, 0.0f, 0.0f }, (vec3){ 0.0f, half_size_w, 0.0f }, green);

    /* Lines around grid. */
    LINE((vec3){ -half_size_w, 0.0f, half_size_h }, (vec3){ half_size_w, 0.0f, half_size_h }, white);
    LINE((vec3){ -half_size_w, 0.0f, -half_size_h }, (vec3){ half_size_w, 0.0f, -half_size_h }, white);

    LINE((vec3){ -half_size_w, 0.0f, half_size_h }, (vec3){ -half_size_w, 0.0f, -half_size_h }, white);
    LINE((vec3){ half_size_w, 0.0f, half_size_h }, (vec3){ half_size_w, 0.0f, -half_size_h }, white);
}

void ddraw_xzgrid(float mins, float maxs, float y, float step) {
    INIT( 2 * CALC(mins,maxs,step) );
    vec3 from, to;
    for( float i = mins; i <= maxs; i += step ) {
        // Horizontal line (along the X)
        vec3set(from, mins, y, i);
        vec3set(to,   maxs, y, i);
        LINE(from, to, gray);

        // Vertical line (along the Z)
        vec3set(from, i, y, mins);
        vec3set(to,   i, y, maxs);
        LINE(from, to, gray);
    }
}

void ddraw_normal(float pos[3], float dir[3], float length) {
    INIT(1);
    float dst[3] = {pos[0]+dir[0]*length, pos[1]+dir[1]*length, pos[2]+dir[2]*length};
    LINE(pos, dst, white);
}

void ddraw_axis(float center[3], float radius) {
    INIT(3);
    vec3 to;

    const float cx = center[0];
    const float cy = center[1];
    const float cz = center[2];

    // Red line: X to X + radius
    vec3set(to, cx + radius, cy, cz);
    LINE(center, to, red);

    // Green line: Y to Y + radius
    vec3set(to, cx, cy + radius, cz);
    LINE(center, to, green);

    // Blue line: Z to Z + radius
    vec3set(to, cx, cy, cz + radius);
    LINE(center, to, blue);
}

void ddraw_cross(float center[3], float radius) {
    INIT(6);
    vec3 pos, neg;

    const float cx = center[0];
    const float cy = center[1];
    const float cz = center[2];
    const float hl = radius * 0.5f; // Half on each side.

    // Cyan/Red line: X - radius/2 to X + radius/2
    vec3set(pos, cx + hl, cy, cz);
    vec3set(neg, cx - hl, cy, cz);
    LINE(center, pos, red);
    LINE(center, neg, cyan);

    // Magenta/Green line: Y - radius/2 to Y + radius/2
    vec3set(pos, cx, cy + hl, cz);
    vec3set(neg, cx, cy - hl, cz);
    LINE(center, pos, green);
    LINE(center, neg, magenta);

    // Yellow/Blue line: Z - radius/2 to Z + radius/2
    vec3set(pos, cx, cy, cz + hl);
    vec3set(neg, cx, cy, cz - hl);
    LINE(center, pos, blue);
    LINE(center, neg, yellow);
}



void ddraw_sphere(vec3 center, float radius) {
    enum { X, Y, Z };
    const int stepSize = 15;
    vec3 cache[360 / 15/*stepSize*/];
    vec3 radiusVec;

    INIT( 2 * CALC(stepSize,360,stepSize) * CALC(stepSize, 360, stepSize) );

    vec3set(radiusVec, 0.0f, 0.0f, radius);

    vec3_add(cache[0], center, radiusVec);
    for (int n = 1; n < sizeof(cache)/sizeof(cache[0]); ++n) {
        vec3cpy(cache[n], cache[0]);
    }

    vec3 lastPoint, temp;
    for (int i = stepSize; i <= 360; i += stepSize) {
        const float s = sinf(i degrees_);
        const float c = cosf(i degrees_);

        lastPoint[X] = center[X];
        lastPoint[Y] = center[Y] + radius * s;
        lastPoint[Z] = center[Z] + radius * c;

        for (int n = 0, j = stepSize; j <= 360; j += stepSize, ++n) {
            temp[X] = center[X] + sinf(j degrees_) * radius * s;
            temp[Y] = center[Y] + cosf(j degrees_) * radius * s;
            temp[Z] = lastPoint[Z];

            LINE(lastPoint, temp, magenta);
            LINE(lastPoint, cache[n], magenta);

            vec3cpy(cache[n], lastPoint);
            vec3cpy(lastPoint, temp);
        }
    }

}

static
void ddraw_box_(vec3 points[8]) {
    INIT( 3 * 4 );
    // Build the lines from points using clever indexing tricks:
    // (& 3 is a fancy way of doing % 4, but avoids the expensive modulo operation)
    for( int i = 0; i < 4; ++i ) {
        LINE(points[i], points[(i + 1) & 3], cyan);
        LINE(points[4 + i], points[4 + ((i + 1) & 3)], cyan);
        LINE(points[i], points[4 + i], cyan);
    }
}

void ddraw_box(vec3 center, float width, float height, float depth ) {
    const float cx = center[0];
    const float cy = center[1];
    const float cz = center[2];
    const float w  = width  * 0.5f;
    const float h  = height * 0.5f;
    const float d  = depth  * 0.5f;

    // Create all the 8 points:
    vec3 points[8];
    #define DDRAW_BOX_POINT(v, op1, op2, op3) \
        { v[0] = cx op1 w; v[1] = cy op2 h; v[2] = cz op3 d; }
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

    vec3cpy(bb[0], minbb);
    vec3cpy(bb[1], maxbb);

    // Expand min/max bounds:
    enum { X,Y,Z };
    for (int i = 0; i < 8; ++i) {
        points[i][X] = bb[(i ^ (i >> 1)) & 1][X];
        points[i][Y] = bb[     (i >> 1)  & 1][Y];
        points[i][Z] = bb[     (i >> 2)  & 1][Z];
    }

    ddraw_box_(points);
}

void ddraw_circle(float center[3], float normal[3], float radius) {
    const float segments = 32;

    vec3 x, y;
    vec3basis(x, y, normal);
    vec3scale(x, radius);
    vec3scale(y, radius);

    vec3 prev;
    for( int i = 0; i <= segments; ++i ) {
        float t = ((M_PI*2.0f) / segments) * i;
        float c = cosf(t), s = sinf(t);
        vec3 point;
        vec3set(point, 
            center[0] + x[0] * c + y[0] * s,
            center[1] + x[1] * c + y[1] * s,
            center[2] + x[2] * c + y[2] * s
        );
        if (i != 0) {
            ddraw_line(prev, point);
        }
        vec3cpy(prev, point);
    }
}

void ddraw_ring(float center[3], float normal[3], float radius) {
    ddraw_circle( center, normal, radius );
    ddraw_circle( center, normal, radius * 0.75f );
}


void ddraw_cone(float from[3], float to[3], float radius) {
    const float bars = 12;

    vec3 n; // normalize(from-to)
    vec3normdir(n, from, to);

    vec3 x, y;
    vec3basis(x, y, n);

    ddraw_circle(to, n, radius);
    for (int i=0; i<bars; ++i) {
        float angle = (i / bars) * 2 * M_PI;
        float c = cosf(angle), s = sinf(angle);
        vec3 end;
        vec3set(end,
            to[0] + radius*c*x[0] + radius*s*y[0],
            to[1] + radius*c*x[1] + radius*s*y[1],
            to[2] + radius*c*x[2] + radius*s*y[2]
        );
        ddraw_line(from, end);
    }
}

void ddraw_arrow(float from[3], float to[3]) {
    vec3 diff;
    vec3_sub(diff, from, to);
    float len = vec3_len(diff);

    const float head_height = len / 3, head_radius = len / 6;

    // to2 = to - n * head_height
    vec3 n;
    vec3normdir(n, to, from);
    vec3scale(n, head_height);
    vec3 to2;
    vec3cpy(to2, to);
    vec3sub(to2, n);

    ddraw_line(from, to);
    ddraw_cone(to, to2, head_radius);
}

void ddraw_sphere2(float center[3], float radius) {
    ddraw_circle(center, (vec3){1,0,0}, radius);
    ddraw_circle(center, (vec3){0,1,0}, radius);
    ddraw_circle(center, (vec3){0,0,1}, radius);
}

void ddraw_frustum(float projview[16]) {
    mat4x4 inverse_projview_; // clip matrix
    mat4x4_invert(inverse_projview_, *((mat4x4*)projview));
    float *inverse_projview = &inverse_projview_[0][0];


    // Start with the standard clip volume, then bring it back to world space.
    static const float planes[8][3] = {
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
        wCoords[i] = matTransformPointXYZW2(points[i], planes[i], inverse_projview);
    }

    enum {X,Y,Z,W};

    // Divide by the W component of each:
    for( int i = 0; i < 8; ++i ) {
        const float epsilon = 1e-9f;

        // But bail if any W ended up as zero.
        if (wCoords[W] < epsilon || wCoords[W] < -epsilon) {
            return;
        }

        points[i][X] /= wCoords[i];
        points[i][Y] /= wCoords[i];
        points[i][Z] /= wCoords[i];
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

void ddraw_text2d( float center[2], const char *fmt, ... ) {
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
    mat4x4_identity(r->tf.matrix);
#if 1
    mat4x4_scale_aniso(r->tf.matrix, r->tf.matrix, scale / window_get(WINDOW_WIDTH)[0], scale / window_get(WINDOW_HEIGHT)[0], 1);
    mat4x4_translate_in_place( r->tf.matrix, center[0], center[1], 0 );
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
    mat4x4 proj;
    ortho(proj, ORTHO_CENTERED | ORTHO_NORMALIZED );
    renderer_enable(&fonts[ddraw_font], &proj[0][0]);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE); // additive: 1 * src_color + 1 * fb_color
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // decal: src_color.a * src_color + (1 - src_color.a) * fb_color
    //glBlendFunc(GL_DST_COLOR, GL_ZERO); // modulate: fb_color * src_color + 0 * fb_color

    for( int i = 0; i < instanced_shapes; ++i ) {
        draw(&shapes[i], &shapes[i].tf.matrix[0][0]);
    }

    // update console
    ortho(proj, ORTHO_TOPLEFT | ORTHO_NORMALIZED);
    renderer_enable(&fonts[ddraw_font], &proj[0][0]);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE); // additive: 1 * src_color + 1 * fb_color
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // decal: src_color.a * src_color + (1 - src_color.a) * fb_color
    //glBlendFunc(GL_DST_COLOR, GL_ZERO); // modulate: fb_color * src_color + 0 * fb_color

    float spacing = fonts[ddraw_font].spaceY;
    for(int l = ddraw__line; l < ddraw__line + 16; ++l) {
        int p = l - ddraw__line;
        int i = l % 16;
        if( ddraw__console[i] ) {
            mat4x4 m;
            mat4x4_identity(m);
            float scale = 3;
            mat4x4_scale_aniso(m, m, scale / window_get(WINDOW_WIDTH)[0], scale / window_get(WINDOW_HEIGHT)[0], 1);
            mat4x4_translate_in_place( m, 0, -p * spacing,0 );
            renderable_t r = {0};
            text( &r, ddraw_font, ddraw__console[i] );
            draw( &r, &m[0][0]);
            renderable_destroy(&r);
        }
    }

    ddraw_clear();
}

#endif

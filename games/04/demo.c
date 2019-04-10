#include "engine.h"
#include <stdlib.h>
#include <stdio.h>

const char* vertex_shader =
    VS150 //VS130
    "uniform mat4 MVP;\n"
    "attribute vec2 att_pos;\n"
    "attribute vec3 att_col;\n"
    "varying vec3 color;\n"
    "void main() {\n"
    "    gl_Position = MVP * vec4(att_pos, 0.0, 1.0);\n"
    "    color = att_col;\n"
    "}\n";

const char* fragment_shader =
    FS150 //FS130
    "varying vec3 color;\n"
    "void main() {\n"
    "    fragColor = vec4(color, 1.0);\n"
    "}\n";

unsigned indices[1]; // dummy
struct {
    float x, y;
    float r, g, b;
} vertices[3] = {
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

EXPORT
int main(int argc, char **argv) {
    puts(";; hello from game 04");

    window_create( 25.f /*25%*/, 0 );

    unsigned program = shader2( vertex_shader, fragment_shader, "att_pos,att_col" );

    int flags = 0;
    mesh2 m;
    mesh2_create(&m, "p2 c3", 3,vertices, 0,indices, flags );

    while (window_update()) {
        // model
        mat44 M;
        rotation44(M, deg((float)now_ms()/1000), 0,0,1 );

        // view
        mat44 V;
        identity44(V);

        // proj
        mat44 P;
        ortho44(P, -window_aspect(), window_aspect(), -1.f, 1.f, 1.f, -1.f);

        // modelviewproj
        mat44 MVP;
        multiply344(MVP, P, V, M);

        // draw
        shader_bind_mat44(program, "MVP", MVP);
        mesh2_render(&m, 0);

        static void *pixels = 0;
        window_swap( &pixels );
        network_sendbuf( pixels, window_width(), window_height(), 3, 7755 );
    }

    window_destroy();
}

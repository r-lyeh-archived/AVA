#include "engine.h"

EXPORT
int main(int argc, char **argv) {
    window_create( 25.f /*25%*/, 0 );

    // instance shader
    const char* vertex_shader =
        VS130 //VS130
        "uniform mat4 MVP;\n"
        "attribute vec2 att_pos;\n"
        "attribute vec3 att_col;\n"
        "varying vec3 color;\n"
        "void main() {\n"
        "    gl_Position = MVP * vec4(att_pos, 0.0, 1.0);\n"
        "    color = att_col;\n"
        "}\n";
    const char* fragment_shader =
        FS130 //FS130
        "varying vec3 color;\n"
        "void main() {\n"
        "    fragColor = vec4(color, 1.0);\n"
        "}\n";
    unsigned program = shader2( vertex_shader, fragment_shader, "att_pos,att_col" );

    // instance mesh
    unsigned indices[3] = {0,1,2};
    struct { float x,y, r,g,b; } vertices[3] = {
        { -0.6f, -0.4f, 1.f, 0.f, 0.f },
        {  0.6f, -0.4f, 0.f, 1.f, 0.f },
        {   0.f,  0.6f, 0.f, 0.f, 1.f }
    };
    mesh2 m; mesh2_create(&m, "p2 c3", 3,vertices, 3,indices, 0/*flags*/ );

    while (window_update()) {
        ddraw_printf("%s", window_timings());
        ddraw_printf("shaders, mesh, matrices & remote rendering");

        // model, view, proj and modelviewproj
        mat44 M,V,P,MVP;
        rotation44(M, deg(time_ms()/1000.f), 0,0,1 );
        identity44(V);
        ortho44(P, -window_aspect(), window_aspect(), -1.f, 1.f, 1.f, -1.f);
        multiply344(MVP, P, V, M);

        // draw
        shader_bind_mat44(program, "MVP", MVP);
        mesh2_render(&m, 0);

        // swap & capture
        static void *pixels = 0;
        window_swap( &pixels );

        // send capture to remote view
        network_sendbuf( pixels, window_width(), window_height(), 3, 7755 );
    }
}

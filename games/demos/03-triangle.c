// inspiration from glfw sample (https://github.com/glfw/glfw/blob/master/examples/simple.c)
// - rlyeh, public domain.

#include <engine.h>

EXPORT
int main(int argc, char **argv) {
    // 25%, no flags
    window_create( 25.f, 0 );

    // instance shader
    const char* vertex =
        VS120
        "uniform mat4 M, VP;\n"
        "attribute vec2 att_pos;\n"
        "attribute vec3 att_col;\n"
        "varying vec3 color;\n"
        "void main() {\n"
        "    gl_Position = VP * M * vec4(att_pos, 0.0, 1.0);\n"
        "    color = att_col;\n"
        "}\n";
    const char* fragment =
        FS120
        "varying vec3 color;\n"
        "void main() {\n"
        "    fragColor = vec4(color, 1.0);\n"
        "}\n";
    unsigned program = shader2( vertex, fragment, "att_pos,att_col" );

    // instance mesh
    unsigned indices[3] = {0,1,2};
    struct { float px,py, cr,cg,cb; } vertices[3] = {
        { -0.6f, -0.4f, 1.f, 0.f, 0.f },
        {  0.6f, -0.4f, 0.f, 1.f, 0.f },
        {   0.f,  0.6f, 0.f, 0.f, 1.f }
    };
    mesh m; mesh_create(&m, "p2 c3", 0,3,vertices, 3,indices, 0/*flags*/ );

    float timer = 0;

    while (window_update()) {
        ddraw_printf("%s", window_stats());
        ddraw_printf("shaders, mesh, matrices & remote rendering");

        // model
        mat44 M;
        rotation44(M, deg(timer), 0,0,1 );
        timer = time_ms() / 1000.f;

        // proj, view and projview
        mat44 P,V,VP;
        identity44(V);
        ortho44(P, -window_aspect(), window_aspect(), -1.f, 1.f, 1.f, -1.f);
        multiply44(VP, P, V);

        // draw
        shader_bind_mat44(program, "M", M);
        shader_bind_mat44(program, "VP", VP);
        mesh_render(&m, 0);

        // swap & capture screen
        static void *pixels = 0;
        window_swap( &pixels );

        // send screen capture to remote viewer (editor). 3 (components), 7755 (YCoCg format 3bytes/2pixels)
        network_sendbuf( pixels, window_width(), window_height(), 3, 7755 );
    }
}

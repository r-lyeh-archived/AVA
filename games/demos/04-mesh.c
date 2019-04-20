// Original sample by Constantine Tarasenkov (unlicensed)
// - rlyeh, public domain.

#include <engine.h>

EXPORT
int main() {
    window_create( 50.f, WINDOW_MSAA4 ); // 50%, AAx4

    const char *vert =
        VS130
        "uniform mat4 u_M, u_VP;\n"
        "attribute vec3 att_vertexpos;\n"
        "out vec4 color;\n"
        "void main() {\n"
        "  gl_Position = u_VP * u_M * vec4(att_vertexpos, 1.0);\n"
        "  color = vec4(gl_VertexID * 0.00027, 0.0, 0.1, 1.0);\n"
        "}\n";
    const char *frag = 
        FS120
        "in vec4 color;\n"
        "void main() {\n"
        "  fragColor = color;\n" // vec4(1);\n"
        "}\n";
    unsigned program = shader2(vert, frag, "att_vertexpos");

    material mat = {0};
    material_create(&mat);
    mat.texture = 0;
    mat.shader = program;
    mat.u_M = glGetUniformLocation(program, "u_M" ); // @fixme
    mat.u_VP = glGetUniformLocation(program, "u_VP" ); // @fixme
    mat.u_texture = -1;
    mat.wireframe = 0;
    mat.two_sided = 0;
    mat.normals_inverted = 1;

#if 0
    mesh m;
    mesh_loadfile(&m, "assets/suzanne.bob");
#else
    mesh2 mesh2 = mesh2_load_obj(coord_system(axis_right, axis_up, axis_back), vfs_find("04-mesh.inl"));
    mesh m = mesh2_as_mesh1( &mesh2, "p3" );
#endif

    camera cam;
    camera_create(&cam, 0.25f, false /*invert_x*/, true /*invert_y*/);
    cam.position = vec3(0,0,-3);

    viewport_color(vec3(0.1/2,0.1/2,0.1)); // vec3(0,0.75f,1));

    while (window_update()) {
        viewport_clear(true, true);
        viewport_clip(vec2(0,0), window_size());

        mat44 proj;
        perspective44(proj, 70, window_aspect(), 0.01, 1000);

        int active = mouse('L') || mouse('R');
        set_mouse( active ? 'hide' : 'show');

        float speed = 0.1f;
        camera_enable(&cam, active);
        camera_fps(&cam, scale3(key_wasdec(), speed), mouse_xy() );

        // obj transform & draw
        mat44 VP; multiply44(VP, proj, cam.view);
        material_enable(&mat, VP);

        mat44 M; identity44(M);
        shader_bind_mat44(program, "u_M", M);
        material_enable(&mat, VP);
        mesh_render(&m, 0);

        ddraw_printf(window_stats());
        ddraw_printf("mesh loading and fly camera");
        window_swap(0);
    }
}

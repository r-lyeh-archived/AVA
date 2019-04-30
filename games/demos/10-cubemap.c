// Original code by @ands (public domain) - https://github.com/ands/spherical_harmonics_playground
// - rlyeh, public domain.

#include <engine.h>

int SKY_DIR = 1;
const char *SKY_DIRS[] = {
    "assets/cubemap/colors/",
    "assets/cubemap/bridge3/",
    "assets/cubemap/room/",
    "assets/cubemap/tantolunden2/",
};

int OBJ_MDL = 0;
const char *OBJ_MDLS[] = {
    "assets/mesh/sphere.obj",
    "assets/mesh/suzanne.obj",
    "assets/mesh/gazebo.obj",
};

typedef struct scene {
    bool init;

    struct {
        GLuint program;
        GLint u_mvp;
        GLint u_cubemap;

        mesh m;
        cubemap c;
    } sky;

    struct {
        GLuint program;
        GLint u_mvp;
        GLint u_coefficients_sh;

        mesh m;
        cubemap c;
    } mesh;
} scene;

static int scene_create(scene *scene) {
    // sky mesh
    vec3 vertices[] = {{+1,-1,+1},{+1,+1,+1},{+1,+1,-1},{-1,+1,-1},{+1,-1,-1},{-1,-1,-1},{-1,-1,+1},{-1,+1,+1}};
    unsigned indices[] = { 0, 1, 2, 3, 4, 5, 6, 3, 7, 1, 6, 0, 4, 2 };
    mesh_create(&scene->sky.m, "p3", 0,countof(vertices),vertices, countof(indices),indices, MESH2_TRIANGLE_STRIP);

    // sky program
    const char *skyVP =
        VS130
        "in vec3 att_position;\n"
        "out vec3 direction;\n"

        "uniform mat4 u_mvp;\n"

        "void main() {\n"
        "    vec4 position = u_mvp * vec4(att_position, 0.0);\n"
        "    gl_Position = position.xyww;\n"
        "    direction = att_position;\n"
        "}\n";

    const char *skyFP =
        FS130
        "in vec3 direction;\n"
        "out vec4 color;\n"

        "uniform samplerCube u_cubemap;\n"

        "void main() {\n"
        "    color = vec4(texture(u_cubemap, direction).rgb, 1.0);\n"
        "}\n";

    scene->sky.program = shader2(skyVP, skyFP, "att_position");
    scene->sky.u_mvp = glGetUniformLocation(scene->sky.program, "u_mvp");
    scene->sky.u_cubemap = glGetUniformLocation(scene->sky.program, "u_cubemap");

    // ---

    const char *skyTextureFiles[6];
    skyTextureFiles[0] = vfs_find(va("%s/posx.jpg", SKY_DIRS[SKY_DIR]));
    skyTextureFiles[1] = vfs_find(va("%s/negx.jpg", SKY_DIRS[SKY_DIR]));
    skyTextureFiles[2] = vfs_find(va("%s/posy.jpg", SKY_DIRS[SKY_DIR]));
    skyTextureFiles[3] = vfs_find(va("%s/negy.jpg", SKY_DIRS[SKY_DIR]));
    skyTextureFiles[4] = vfs_find(va("%s/posz.jpg", SKY_DIRS[SKY_DIR]));
    skyTextureFiles[5] = vfs_find(va("%s/negz.jpg", SKY_DIRS[SKY_DIR]));

    scene->sky.c = cubemap_load( skyTextureFiles, 0 );

    // mesh
    coord_system cs = { axis_right, axis_up, axis_front };
    static mesh2 m2; m2 = mesh2_load_obj(cs, vfs_find(OBJ_MDLS[OBJ_MDL]));
    scene->mesh.m = mesh2_as_mesh1(&m2, "p3 n3");

    // copy sh coefficients from sky cubemap
    scene->mesh.c = scene->sky.c;

    const char *vp =
        VS130
            "in vec3 att_position;\n"
            "in vec3 att_normal;\n"
            "out vec3 normal;\n"

            "uniform mat4 u_mvp;\n"

            "void main() {\n"
            "    gl_Position = u_mvp * vec4(att_position, 1.0);\n"
            "    normal = att_normal;\n"
            "}\n";
    const char *fp =
        FS130
            "in vec3 normal;\n"
            "out vec4 color;\n"

            "uniform vec3 u_coefficients_sh[9];\n"

            "void main() {\n"
            "    vec3 n = /*normalize*/(normal);\n"
            "    vec3 SHLightResult[9];\n"
            "    SHLightResult[0] =  0.282095f * u_coefficients_sh[0];\n"
            "    SHLightResult[1] = -0.488603f * u_coefficients_sh[1] * n.y;\n"
            "    SHLightResult[2] =  0.488603f * u_coefficients_sh[2] * n.z;\n"
            "    SHLightResult[3] = -0.488603f * u_coefficients_sh[3] * n.x;\n"
            "    SHLightResult[4] =  1.092548f * u_coefficients_sh[4] * n.x * n.y;\n"
            "    SHLightResult[5] = -1.092548f * u_coefficients_sh[5] * n.y * n.z;\n"
            "    SHLightResult[6] =  0.315392f * u_coefficients_sh[6] * (3.0f * n.z * n.z - 1.0f);\n"
            "    SHLightResult[7] = -1.092548f * u_coefficients_sh[7] * n.x * n.z;\n"
            "    SHLightResult[8] =  0.546274f * u_coefficients_sh[8] * (n.x * n.x - n.y * n.y);\n"
            "    vec3 result = vec3(0.0);\n"
            "    for (int i = 0; i < 9; ++i)\n"
            "        result += SHLightResult[i];\n"
            "    color = vec4(result, 1.0);\n"
            "}\n";

    scene->mesh.program = shader2(vp, fp, "att_position,att_normal");
    scene->mesh.u_mvp = glGetUniformLocation(scene->mesh.program, "u_mvp");
    scene->mesh.u_coefficients_sh = glGetUniformLocation(scene->mesh.program, "u_coefficients_sh");

    scene->init = 1;
    return 1;
}

static void scene_draw(scene *scene, float *mvp) {
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glDisable(GL_CULL_FACE);

    // mesh
    glDepthMask(GL_TRUE);
    glUseProgram(scene->mesh.program);
    glUniformMatrix4fv(scene->mesh.u_mvp, 1, GL_FALSE, mvp);
    glUniform3fv(scene->mesh.u_coefficients_sh, 9, &scene->mesh.c.sh[0].x);
    mesh_render(&scene->mesh.m, 0);
    // glBindVertexArray(scene->mesh.m.vao);
    // glDrawArrays(GL_TRIANGLES, 0, scene->mesh.m.vertices);

    // sky
    glDepthMask(GL_FALSE);
    glUseProgram(scene->sky.program);
    glUniformMatrix4fv(scene->sky.u_mvp, 1, GL_FALSE, mvp);
    glUniform1i(scene->sky.u_cubemap, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, scene->sky.c.texture);
    mesh_render(&scene->sky.m, 0);
}

static void scene_destroy(scene *scene) {
    scene->init = 0;

    // sky
    glDeleteProgram(scene->sky.program);
    mesh_destroy(&scene->sky.m);
    cubemap_destroy(&scene->sky.c);

    // mesh
    glDeleteProgram(scene->mesh.program);
    mesh_destroy(&scene->mesh.m);
}


int main(int argc, char** argv) {
    window_create(50,0);

    camera cam;
    camera_create(&cam, 0.25f, false, true);
    camera_teleport(&cam, vec3(3.97,0.45,1.55), vec2(-520,-7));

    int must_reload = 0;
    scene scene = {0};

    while( window_update()) {

        viewport_clear( false, true );
        viewport_clip( vec2(0,0), window_size() );

        if( key_down('1') ) SKY_DIR = 0, must_reload = 1;
        if( key_down('2') ) SKY_DIR = 1, must_reload = 1;
        if( key_down('3') ) SKY_DIR = 2, must_reload = 1;
        if( key_down('4') ) SKY_DIR = 3, must_reload = 1;

        if( key_down('F1') ) OBJ_MDL = 0, must_reload = 1;
        if( key_down('F2') ) OBJ_MDL = 1, must_reload = 1;
        if( key_down('F3') ) OBJ_MDL = 2, must_reload = 1;

        if( must_reload || key_down('F5') ) {
            must_reload = 0;
            scene_destroy(&scene);
        }
        if( !scene.init ) {
            if (!scene_create(&scene)) {
                PANIC("Could not initialize scene.");
            }
        }

        bool active = !!mouse('L') || !!mouse('R');
        set_mouse( active ? 'hide' : 'show' );
        camera_enable( &cam, active );

        float view[16]; camera_fps(&cam, scale3(key_wasdec(), 0.1f), mouse_xy() ); copy44(view, cam.view);
        float proj[16]; perspective44(proj, 45, window_aspect(), 0.01f, 100.0f);
        float mvp[16]; multiply44(mvp, proj, view);

        scene_draw(&scene, mvp);

        ddraw_printf(window_stats());
        ddraw_printf("%f,%f,%f %f,%f", cam.position.x,cam.position.y,cam.position.z, cam.yaw,cam.pitch);
        ddraw_printf("1..4: change cubemap, f1..f3: change model");

        window_swap(0);
    }

    scene_destroy(&scene);
}

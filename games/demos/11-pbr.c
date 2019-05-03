// Original code by @sgorsten (unlicense)
// - rlyeh, public domain.

#include <engine.h>

// ---

typedef struct vertex { vec3 position, normal; } vertex;

vertex make_vertex(int slices, int stacks, float radius, int i, int j) {
    const float tau = 6.2831853f, longitude = i*tau/slices, latitude = (j-(stacks*0.5f))*tau/2/stacks;
    const vec3 normal = vec3(cos(longitude)*cos(latitude), sin(latitude), sin(longitude)*cos(latitude)); // Poles at +/-y
    vertex v = { scale3(normal,radius), normal };
    return v;
}

array(vertex) make_sphere(int slices, int stacks, float radius) {
    array(vertex) vertices = {0};
    for(int i=0; i<slices; ++i) {
        for(int j=0; j<stacks; ++j) {
            array_push(vertices, make_vertex(slices, stacks, radius, i,j));
            array_push(vertices, make_vertex(slices, stacks, radius, i,j+1));
            array_push(vertices, make_vertex(slices, stacks, radius, i+1,j+1));

            array_push(vertices, make_vertex(slices, stacks, radius, i+1,j+1));
            array_push(vertices, make_vertex(slices, stacks, radius, i+1,j));
            array_push(vertices, make_vertex(slices, stacks, radius, i,j));
        }
    }
    return vertices;
}

// ---

API unsigned char *stbi_load(const char *,int *,int *,int *,int);
API float *stbi_loadf(const char *,int *,int *,int *,int);
API void stbi_set_flip_vertically_on_load(int);
API void stbi_image_free(void *);

GLuint load_gl_texture(const char *filename)
{
    int width, height;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* pixels = stbi_load(filename, &width, &height, NULL, 3);
    GLuint tex;
    //glGenTextures(1, &tex);
    //glBindTexture(GL_TEXTURE_2D, tex);
    glCreateTextures(GL_TEXTURE_2D, 1, &tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(pixels);
    printf("Loaded %s (%dx%d) (id:%d)\n", filename, width, height, tex);
    return tex;
}

// ---

int main() {
    window_create(0.50f, WINDOW_MSAA4);

    pbr_tools tools = make_pbr();
    unsigned vs1 = compile_shader('v', 2, preamble, vert_shader_source);
    unsigned fs1 = compile_shader('f', 3, preamble, pbr_lighting, frag_shader_source);
    unsigned prog = make_program(vs1, fs1);

    unsigned vs2 = compile_shader('v', 2, preamble, vert_shader_source);
    unsigned fs2 = compile_shader('f', 3, preamble, pbr_lighting, textured_frag_shader_source);
    unsigned texprog = make_program(vs2, fs2);

    glEnable(GL_CULL_FACE); glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    // Convert spheremap(s) to cubemap(s)
    const GLuint brdf_integration_map = pbr_compute_brdfmap(&tools);
    const environment env[2] = {
        load_enviroment(&tools, vfs_find("factory-catwalk.hdr")),
//      load_enviroment(&tools, vfs_find("monument-valley.hdr")),
    };
    int env_index = 0;

    // Load model(s)
    const coord_system coords = {axis_right, axis_up, axis_front};
    mesh2 model2 = mesh2_load_obj(coords, vfs_find("helmet-mesh.obj")); //array(mesh2) fbx = load_meshes_from_fbx(coords, "file.fbx");
    mesh model = mesh2_as_mesh1(&model2, "p3 n3 t2 a3 i3");
    GLuint tex_albedo = load_gl_texture(vfs_find("helmet-albedo.jpg"));
    GLuint tex_normal = load_gl_texture(vfs_find("helmet-normal.jpg"));
    GLuint tex_metallic = load_gl_texture(vfs_find("helmet-metallic.jpg"));
    GLuint tex_roughness = load_gl_texture(vfs_find("helmet-roughness.jpg"));

    mesh sphere;
    array(vertex) sphere_verts = make_sphere(32, 16, 0.4f);
    mesh_create(&sphere, "p3 n3", 0,array_count(sphere_verts),sphere_verts, 0,NULL, 0);

    // Initialize camera
    camera cam;
    camera_create(&cam, 0.25f, false, true); // fps
    camera_teleport(&cam, vec3(-5,0,-13), vec2(70,-5));

    // get ready
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(GL_DEPTH_TEST);

    while(window_update()) {
        // input
        if(key_down('1')) env_index = 0;
        // if(key_down('2')) env_index = 1;

        camera_enable(&cam, !!mouse('L'));
        camera_fps(&cam, key_wasdec(), mouse_xy());

        // clear scene
        glViewport(0, 0, window_width(), window_height());
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); // color unlikely needed here

        // setup matrices
        mat44 view_matrix; copy44(view_matrix, cam.view);
        mat44 proj_matrix; perspective44(proj_matrix, 50, window_aspect(), 0.1f, 100.f);

        // Render skybox
            glDisable(GL_CULL_FACE); glDepthMask(GL_FALSE); // glFrontFace(GL_CW);
            mat44 skybox_V;  rotationq44(skybox_V, conjq(eulerq(vec3(-cam.yaw/*cam.pitch*/,0,-cam.pitch/*cam.yaw*/))));
            mat44 skybox_VP; multiply44(skybox_VP, proj_matrix, skybox_V);
            pbr_draw_skybox(&tools, env[env_index].environment_cubemap, skybox_VP);
            glEnable(GL_CULL_FACE); glDepthMask(GL_TRUE); // glFrontFace(GL_CCW);

glDisable(GL_CULL_FACE);

        // Render Helmet
        if(1) {
            mat44 VP2; multiply44(VP2, proj_matrix, view_matrix);

            use(texprog);
            uniform16f(texprog, "u_view_proj_matrix", VP2);
            uniform3f(texprog, "u_eye_position", cam.position);

            uniform1f(texprog, "u_ambient_occlusion", 1.0f);

            // Set up pbr lighting (bindings 0,1,2)
            bind_texture(texprog, "u_brdf_integration_map", brdf_integration_map);
            bind_texture(texprog, "u_irradiance_map", env[env_index].irradiance_cubemap);
            bind_texture(texprog, "u_reflectance_map", env[env_index].reflectance_cubemap);
            // Set up textures (bindings 3,4,5,6)
            bind_texture(texprog, "u_albedo_tex", tex_albedo);
            bind_texture(texprog, "u_normal_tex", tex_normal);
            bind_texture(texprog, "u_roughness_tex", tex_roughness);
            bind_texture(texprog, "u_metalness_tex", tex_metallic);

            // Render helmet
            //for(auto & mesh : helmet_fbx)
            {
                //skeletal meshes only: texprog.uniform("u_model_matrix", mul(mesh.bones[0].initial_pose.get_local_transform(), mesh.bones[0].model_to_bone_matrix));
                mat44 identity; identity44(identity);
                uniform16f(texprog, "u_model_matrix", identity );

                mesh_render(&model, 0);
            }
        }

        // Render spheres
        if(1) {
            use(prog);
            { mat44 VP; multiply44(VP, proj_matrix, view_matrix); uniform16f(prog, "u_view_proj_matrix", VP); }
            uniform3f(prog, "u_eye_position", cam.position);

            uniform1f(prog, "u_ambient_occlusion", 1.0f);

            // Set up pbr lighting
            bind_texture(prog, "u_brdf_integration_map", brdf_integration_map);
            bind_texture(prog, "u_irradiance_map", env[env_index].irradiance_cubemap);
            bind_texture(prog, "u_reflectance_map", env[env_index].reflectance_cubemap);

            // Render spheres
            for(int i=0; i<7; ++i) {
                for(int j=0; j<7; ++j) {
                    for(int k=0; k<7; ++k) {
                        mat44 M; translation44(M, j-3.0f, i-3.0f, k-3.0f);
                        uniform16f(prog, "u_model_matrix", (M));

                        vec3 albedos[] = {{1,1,1}, {1,0,0}, {1,1,0}, {0,1,0}, {0,1,1}, {0,0,1}, {1,0,1}};
                        uniform3f(prog, "u_albedo", albedos[k]);
                        uniform1f(prog, "u_metalness", 1-(i+0.5f)/7);
                        uniform1f(prog, "u_roughness", (j+0.5f)/7);

                        mesh_render(&sphere, 0);
                    }
                }
            }
        }

        ddraw_printf(window_stats());
        window_swap(0);
    }

    array_free(sphere_verts);
}

#include <engine.h>

int main(int argc, char **argv) {
    window_create(0.50f, 0);

    // create cubes
    unsigned program = shader2(
        VS130
            "in vec3 att_position;\n"
            "in vec4 att_color;\n"
            "out vec4 color;\n"
            "uniform mat4 u_mvp;\n"
            "void main() {\n"
            "    gl_Position = u_mvp * vec4(att_position, 1.0f);\n"
            "    color = att_color;\n"
            "}\n",
        FS130
            "in vec4 color;\n"
            "out vec4 out_color;\n"
            "void main() {\n"
            "   out_color = color;\n"
            "}\n",
        "att_position,att_color" );

    // 1-unit length, p3 c4 [36] vertex cube
    struct { vec3 p; vec4 c; } vertex[36] = {
        vec3(-0.5f,-0.5f,-0.5f), vec4(0,0,0,0),
        vec3( 0.5f,-0.5f,-0.5f), vec4(0,0,0,0),
        vec3( 0.5f, 0.5f,-0.5f), vec4(0,0,0,0),
        vec3( 0.5f, 0.5f,-0.5f), vec4(0,0,0,0),
        vec3(-0.5f, 0.5f,-0.5f), vec4(0,0,0,0),
        vec3(-0.5f,-0.5f,-0.5f), vec4(0,0,0,0),

        vec3(-0.5f,-0.5f, 0.5f), vec4(0,0,0,0),
        vec3( 0.5f,-0.5f, 0.5f), vec4(0,0,0,0),
        vec3( 0.5f, 0.5f, 0.5f), vec4(0,0,0,0),
        vec3( 0.5f, 0.5f, 0.5f), vec4(0,0,0,0),
        vec3(-0.5f, 0.5f, 0.5f), vec4(0,0,0,0),
        vec3(-0.5f,-0.5f, 0.5f), vec4(0,0,0,0),

        vec3(-0.5f, 0.5f, 0.5f), vec4(0,0,0,0),
        vec3(-0.5f, 0.5f,-0.5f), vec4(0,0,0,0),
        vec3(-0.5f,-0.5f,-0.5f), vec4(0,0,0,0),
        vec3(-0.5f,-0.5f,-0.5f), vec4(0,0,0,0),
        vec3(-0.5f,-0.5f, 0.5f), vec4(0,0,0,0),
        vec3(-0.5f, 0.5f, 0.5f), vec4(0,0,0,0),

        vec3( 0.5f, 0.5f, 0.5f), vec4(0,0,0,0),
        vec3( 0.5f, 0.5f,-0.5f), vec4(0,0,0,0),
        vec3( 0.5f,-0.5f,-0.5f), vec4(0,0,0,0),
        vec3( 0.5f,-0.5f,-0.5f), vec4(0,0,0,0),
        vec3( 0.5f,-0.5f, 0.5f), vec4(0,0,0,0),
        vec3( 0.5f, 0.5f, 0.5f), vec4(0,0,0,0),

        vec3(-0.5f,-0.5f,-0.5f), vec4(0,0,0,0),
        vec3( 0.5f,-0.5f,-0.5f), vec4(0,0,0,0),
        vec3( 0.5f,-0.5f, 0.5f), vec4(0,0,0,0),
        vec3( 0.5f,-0.5f, 0.5f), vec4(0,0,0,0),
        vec3(-0.5f,-0.5f, 0.5f), vec4(0,0,0,0),
        vec3(-0.5f,-0.5f,-0.5f), vec4(0,0,0,0),

        vec3(-0.5f, 0.5f,-0.5f), vec4(0,0,0,0),
        vec3( 0.5f, 0.5f,-0.5f), vec4(0,0,0,0),
        vec3( 0.5f, 0.5f, 0.5f), vec4(0,0,0,0),
        vec3( 0.5f, 0.5f, 0.5f), vec4(0,0,0,0),
        vec3(-0.5f, 0.5f, 0.5f), vec4(0,0,0,0),
        vec3(-0.5f, 0.5f,-0.5f), vec4(0,0,0,0),
    };

    // randomize color & position

    #ifndef MAX_CUBES
    #define MAX_CUBES 256
    #endif

    mesh cube[MAX_CUBES] = {0};
    vec3 cube_positions[MAX_CUBES] = {0};

    for( int i = 0; i < MAX_CUBES; ++i ) {
        int r = 10 + (rand() % 245);
        int g = 10 + (rand() % 245);
        int b = 10 + (rand() % 245);

        for( int j = 0; j < countof(vertex); ++j ) {
            vertex[j].c.r = r / 255.f;
            vertex[j].c.g = g / 255.f;
            vertex[j].c.b = b / 255.f;
            vertex[j].c.a = 1.f;
        }

        mesh_create(&cube[i], "p3 c4", 0,countof(vertex),vertex, 0,NULL, 0);
        cube_positions[i] = vec3((r-127)/10,(g-127)/10,(b-127)/10);
    }

    // postfx

    postfx fx;
    postfx_create(&fx, 0);

    int slot = 0;
    postfx_load(&fx, slot++, "PassScanlines",          NULL, "shaders/PassScanlines.fs");
    postfx_load(&fx, slot++, "PassFXAA",               NULL, "shaders/PassFXAA.fs");
    postfx_load(&fx, slot++, "PassPixelate",           NULL, "shaders/PassPixelate.fs");
    for( int i = 1; i < argc; ++i) {
        postfx_load( &fx, slot++, argv[i], NULL,argv[i] );
    }

    postfx_enable(&fx, 0, true);

    camera cam;
    camera_create(&cam, 0.25f, false, true);

    while( window_update() ) {

        // toggle postfxs
        if(key_down('\b'))  postfx_clear(&fx);
        if(key_down(  '1')) postfx_toggle(&fx,  0);
        if(key_down(  '2')) postfx_toggle(&fx,  1);
        if(key_down(  '3')) postfx_toggle(&fx,  2);
        if(key_down(  '4')) postfx_toggle(&fx,  3);
        if(key_down(  '5')) postfx_toggle(&fx,  4);
        if(key_down(  '6')) postfx_toggle(&fx,  5);
        if(key_down(  '7')) postfx_toggle(&fx,  6);
        if(key_down(  '8')) postfx_toggle(&fx,  7);
        if(key_down(  '9')) postfx_toggle(&fx,  8);
        if(key_down(  '0')) postfx_toggle(&fx,  9);
        if(key_down( 'F1')) postfx_toggle(&fx, 10);
        if(key_down( 'F2')) postfx_toggle(&fx, 11);
        if(key_down( 'F3')) postfx_toggle(&fx, 12);
        if(key_down( 'F4')) postfx_toggle(&fx, 13);
        if(key_down( 'F5')) postfx_toggle(&fx, 14);
        if(key_down( 'F6')) postfx_toggle(&fx, 15);
        if(key_down( 'F7')) postfx_toggle(&fx, 16);
        if(key_down( 'F8')) postfx_toggle(&fx, 17);
        if(key_down( 'F9')) postfx_toggle(&fx, 18);
        if(key_down('F10')) postfx_toggle(&fx, 19);
        if(key_down('F11')) postfx_toggle(&fx, 20);
        if(key_down('F12')) postfx_toggle(&fx, 21);

        // update camera and set the uniform
        int active = !!mouse('L') || !!mouse('R');
        set_mouse( active ? 'hide' : 'show' );
        camera_enable( &cam, active );
        camera_fps( &cam, key_wasdec(), mouse_xy() );

        mat44 proj;
        perspective44(proj, 45, window_aspect(), 0.1f, 1000);

        char inuse[2048];
        postfx_resize(&fx, window_width(), window_height());
        postfx_begin(&fx, inuse);
            {
                // clear color & depth, and clip viewport
                viewport_clear(true, true);
                viewport_clip(vec2(0,0), window_size());

                // bind and draw cubes
                glEnable(GL_DEPTH_TEST);
                for( int i = 0; i < MAX_CUBES; ++i ) {
                    mat44 model;
                    translation44(model, cube_positions[i].x, cube_positions[i].y, cube_positions[i].z);

                    mat44 mvp;
                    multiply344(mvp, proj, cam.view, model);

                    shader_bind_mat44(program, "u_mvp", mvp);
                    mesh_render(&cube[i], 0/*mvp*/);
                }
            }
        postfx_end(&fx);

        ddraw_printf(window_stats());
        ddraw_printf("Use [0..9] [F1..F12] keys to toggle post-effects. Backspace to clear.");
        ddraw_printf("Shaders in-use: %s", inuse);

        window_swap(0);
    }
}

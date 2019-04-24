// post-effects swap chain.
// - rlyeh, public domain.

#ifndef POSTFX_H
#define POSTFX_H

typedef struct passfx {
    mesh m;
    char *name;
    unsigned program;
    int uniforms[16];
} passfx;

typedef struct postfx {
    // renderbuffers, color & depth textures
    unsigned fb[2];
    texture3 diffuse[2], depth[2];
    // shader passes
    passfx pass[64];
    uint64_t mask;
    // global enable flag
    bool enabled;
} postfx;


API void postfx_create(postfx *fx, int flags);
API void postfx_destroy( postfx *fx );

API bool postfx_load( postfx *fx, int slot, const char *name, const char *vertex, const char *fragment );
API void postfx_resize( postfx *fx, int width, int height );
API bool postfx_begin(postfx *fx, char *list_inuse);
API bool postfx_end(postfx *fx);

API bool postfx_enable(postfx *fx, int pass_number, bool enabled);
API bool postfx_toggle(postfx *fx, int pass_number);
API void postfx_clear(postfx *fx);

#endif

// ----------------------------------------------------------------------------

#ifdef POSTFX_C
#pragma once

enum {
    u_color,
    u_depth,
    u_time,
    u_width,
    u_height,
    u_mousex,
    u_mousey,
};

void postfx_create(postfx *fx, int flags) {
    postfx z = {0};
    *fx = z;
    fx->enabled = 1;
}

void postfx_destroy( postfx *fx ) {
    texture3_destroy(fx->diffuse[0]);
    texture3_destroy(fx->diffuse[1]);
    texture3_destroy(fx->depth[0]);
    texture3_destroy(fx->depth[1]);
    fbo2_destroy(fx->fb[0]);
    fbo2_destroy(fx->fb[1]);
}

bool postfx_load( postfx *fx, int slot, const char *name, const char *vertex, const char *fragment ) {
    passfx *p = &fx->pass[ slot & 63 ];

    p->name = strdup(name); // @todo: fix leak

    // load vertex
    const char *vs = vertex ? file_readz(vfs_find(vertex)) : shader_fullscreen_quad_vs();
    if(!vs) vs = vertex;

    // load fragment
    const char *fs = fragment ? file_readz(vfs_find(fragment)) : fragment;

    // patch fragment
    char *fs2 = (char*)CALLOC(1, 64*1024);
    strcat(fs2, FS130);
    strcat(fs2, "uniform sampler2D iChannel0;\n");
    strcat(fs2, "vec2 iResolution = vec2(width, height);\n");
    strcat(fs2, "vec2 iMouse = vec2(mousex, mousey);\n");
    strcat(fs2, "float iTime = time;\n");
    strcat(fs2, "float iGlobalTime = time;\n");
    strcat(fs2, "in vec2 texcoord;\n");

    if( strstr(fs, "mainImage") ) {
        strcat(fs2,
            "out vec4 fragColor;\n"
            "void mainImage( out vec4 fragColor, in vec2 fragCoord );\n"
            "void main() {\n"
            "   mainImage(fragColor, texcoord.xy * iResolution);\n"
            "}\n");
    }

    strcat(fs2, fs);

    p->program = shader2(vs, fs2, "vtexcoord" );

    FREE(fs2);

    glUseProgram(p->program); // needed?

    for( int i = 0; i < countof(p->uniforms); ++i ) p->uniforms[i] = -1;

    if( p->uniforms[u_time] == -1 )   p->uniforms[u_time] = glGetUniformLocation(p->program, "time");

    if( p->uniforms[u_width] == -1 )  p->uniforms[u_width] = glGetUniformLocation(p->program, "width");
    if( p->uniforms[u_height] == -1 ) p->uniforms[u_height] = glGetUniformLocation(p->program, "height");

    if( p->uniforms[u_mousex] == -1 ) p->uniforms[u_mousex] = glGetUniformLocation(p->program, "mousex");
    if( p->uniforms[u_mousey] == -1 ) p->uniforms[u_mousey] = glGetUniformLocation(p->program, "mousey");

    if( p->uniforms[u_color] == -1 ) p->uniforms[u_color] = glGetUniformLocation(p->program, "tex");
    if( p->uniforms[u_color] == -1 ) p->uniforms[u_color] = glGetUniformLocation(p->program, "tex0");
    if( p->uniforms[u_color] == -1 ) p->uniforms[u_color] = glGetUniformLocation(p->program, "tColor");
    if( p->uniforms[u_color] == -1 ) p->uniforms[u_color] = glGetUniformLocation(p->program, "tDiffuse");
    if( p->uniforms[u_color] == -1 ) p->uniforms[u_color] = glGetUniformLocation(p->program, "iChannel0");

    if( p->uniforms[u_depth] == -1 ) p->uniforms[u_depth] = glGetUniformLocation(p->program, "tex1");
    if( p->uniforms[u_depth] == -1 ) p->uniforms[u_depth] = glGetUniformLocation(p->program, "tDepth");
    if( p->uniforms[u_depth] == -1 ) p->uniforms[u_depth] = glGetUniformLocation(p->program, "iChannel1");

    // set quad
    glGenVertexArrays(1, &p->m.vao);
    return true;
}

void postfx_resize( postfx *fx, int width, int height ) {
    // resize if needed
    if( fx->diffuse[0].width != width || fx->diffuse[0].height != height ) {
        texture3_destroy(fx->diffuse[0]);
        texture3_destroy(fx->diffuse[1]);
        texture3_destroy(fx->depth[0]);
        texture3_destroy(fx->depth[1]);
        fbo2_destroy(fx->fb[0]);
        fbo2_destroy(fx->fb[1]);

        // create texture, set texture parameters and content
        fx->diffuse[0] = texture3_create(0, width, height, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, GL_CLAMP_TO_EDGE, NULL);
        fx->depth[0] = texture3_create(1, width, height, 1, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST, GL_CLAMP_TO_EDGE, NULL);
        fx->fb[0] = fbo2(fx->diffuse[0], fx->depth[0]);

        // create texture, set texture parameters and content
        fx->diffuse[1] = texture3_create(0, width, height, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, GL_CLAMP_TO_EDGE, NULL);
        fx->depth[1] = texture3_create(1, width, height, 1, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST, GL_CLAMP_TO_EDGE, NULL);
        fx->fb[1] = fbo2(fx->diffuse[1], fx->depth[1]);
    }
}

uint64_t postfx_count_ones(uint64_t x) {
    // [src] https://en.wikipedia.org/wiki/Hamming_weight
    x -= (x >> 1) & 0x5555555555555555ULL;                                //put count of each 2 bits into those 2 bits
    x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL); //put count of each 4 bits into those 4 bits 
    x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0fULL;                           //put count of each 8 bits into those 8 bits 
    return (x * 0x0101010101010101ULL) >> 56;                             //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ... 
}

bool postfx_enable(postfx *fx, int pass, bool enabled) {
    fx->mask = enabled ? fx->mask | (1ull << pass) : fx->mask & ~(1ull << pass);
    fx->enabled = !!postfx_count_ones(fx->mask);
    return fx->enabled;
}

bool postfx_toggle(postfx *fx, int pass) {
    return postfx_enable(fx, pass, 1 ^ (!!(fx->mask & (1ull << pass))) );
}

void postfx_clear(postfx *fx) {
    fx->mask = fx->enabled = 0;
}

bool postfx_begin(postfx *fx, char *list) {
    if( list ) {
        *list = 0;
        if( fx->enabled )
        for(int i = 0, e = countof(fx->pass); i < e; ++i ) {
            if( fx->mask & (1ull << i) ) if( fx->pass[i].name ) list += sprintf(list, "%s->", fx->pass[i].name);
        }
    }

    uint64_t num_active_passes = postfx_count_ones(fx->mask);
    bool active = fx->enabled && num_active_passes;
    if( !active ) {
        fbo2_unbind();
        return false;
    }

    fbo2_bind(fx->fb[0]);
    return true;
}

bool postfx_end(postfx *fx) {
    uint64_t num_active_passes = postfx_count_ones(fx->mask);
    bool active = fx->enabled && num_active_passes;
    if( !active ) {
        return false;
    }

    fbo2_unbind();

    // disable depth test in 2d rendering
    glDisable(GL_DEPTH_TEST);

    int frame = 0;
    float t = time_ms() / 1000.f;
    float w = fx->diffuse[0].width;
    float h = fx->diffuse[0].height;
    float mx = mouse_xy().x;
    float my = mouse_xy().y;

    for(int i = 0, e = countof(fx->pass); i < e; ++i) {
        if( fx->mask & (1ull << i) ) {
            passfx *pass = &fx->pass[i];

            if( !pass->program ) { --num_active_passes; continue; }
            glUseProgram(pass->program);

            // bind texture to texture unit 0
            texture3_bind(fx->diffuse[frame]);
            glUniform1i(pass->uniforms[u_color], 0);

            // bind depth to texture unit 1
            texture3_bind(fx->depth[frame]);
            glUniform1i(pass->uniforms[u_depth], 1);

            // bind uniforms
            glUniform1f(pass->uniforms[u_time], t);
            glUniform1f(pass->uniforms[u_width], w);
            glUniform1f(pass->uniforms[u_height], h);

            glUniform1f(pass->uniforms[u_mousex], mx);
            glUniform1f(pass->uniforms[u_mousey], my);

            // bind the vao
            int bound = --num_active_passes;
            if( bound ) fbo2_bind(fx->fb[frame ^= 1]);

                // fullscreen quad
                glBindVertexArray(pass->m.vao);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glBindVertexArray(0);

            if( bound ) fbo2_unbind();
            else glUseProgram(0);
        }
    }

    return true;
}

#endif

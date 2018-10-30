#include "engine.h"
#include <stdlib.h>
#include <stdio.h>

static const struct {
    float x, y;
    float r, g, b;
} vertices[3] = {
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

static const char* vertex_shader_text =
    VS130
    "uniform mat4 MVP;\n"
    "attribute vec3 vCol;\n"
    "attribute vec2 vPos;\n"
    "varying vec3 color;\n"
    "void main() {\n"
    "    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
    "    color = vCol;\n"
    "}\n";
static const char* fragment_shader_text =
    FS130
    "varying vec3 color;\n"
    "void main() {\n"
    "    fragColor = vec4(color, 1.0);\n"
    "}\n";


EXPORT
int main() {
    puts(";; hello from game 04");

    window_create( 25.f /*25%*/, 0 );

    // NOTE: OpenGL error checks have been omitted for brevity
    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint program = shader( vertex_shader_text, fragment_shader_text );

    GLint mvp_location, vpos_location, vcol_location;
    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*) 0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*) (sizeof(float) * 2));

    void *pixels = 0;
    int width, height;
    while (window_update(&width, &height)) {

        mat4 m, p, mvp;
        // model
        mat4_identity(m);
        mat4_rotate_Z(m, m, (float)(now_ms() / 1000.f));
        // proj
        float ratio = width / (float)height;
        mat4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        // mvp
        mat4_mul(mvp, p, m);

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        window_swap( &pixels );
        network_sendbuf( pixels, width, height, 3, 242 );
    }

    window_destroy();
}

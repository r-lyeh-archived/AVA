#ifdef RENDER_C
#pragma once
#define RENDERER_C
#define DDRAW_C
#define FONT_C
#define MESH_C
#define QUAD_C
#define SHADER_C
#define SHAPE_C
#define TEXT_C
#define TEXTURE_C
#define UNICODE_C
#define OPENGL_C
#define PASS_C
#endif

#include <stdlib.h>

#include "render_opengl.c"
#include "render_renderer.c"
#include "render_pass.c"

#include "render_ddraw.c"
#include "render_font.c"
#include "render_mesh.c"
#include "render_quad.c"
#include "render_shader.c"
#include "render_shape.c"
#include "render_text.c"
#include "render_texture.c"
#include "render_unicode.c"

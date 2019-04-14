#ifdef RENDER_C
#pragma once
#define OPENGL_C

#define PASS_C
#define SHAPE_C
#define MESH_C

#define SHADER_C
#define SHADER2_C
#define TEXTURE_C
#define TEXTURE2_C
#define FBO2_C
#define MESH2_C
#define VIEWPORT2_C
#define MATERIAL2_C
#define DDRAW2_C

#define RENDERNODE_C
#endif

#include <stdlib.h>

#include "render_opengl.c"

#include "render_shader.c"
#include "render_shader2.c"
#include "render_texture.c"
#include "render_texture2.c"
#include "render_fbo2.c"
#include "render_mesh.c"
#include "render_viewport2.c"
#include "render_material2.c"
#include "render_mesh2.c"
#include "render_ddraw2.c"

#include "rendernode.c"

// Public domain single-file C implementation by Constantine Tarasenkov
//
// LICENSE
//
// This is free and unencumbered software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
// 
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#ifndef GLFAST_H_
#define GLFAST_H_

#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED 1
#endif

#include "./SDL2/SDL.h" // #include <SDL2/SDL.h>

// GLFAST //////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

// RUST TYPES //////////////////////////////////////////////////////////////////

#ifndef i8
#define i8  int8_t
#define i16 int16_t
#define i32 int32_t
#define i64 int64_t
#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define f16 uint16_t
#define f32 float
#define f64 double
#endif

// OPENGL //////////////////////////////////////////////////////////////////////

#ifdef GLFAST_EXTERNAL_LOADER

#define GL_MAP_PERSISTENT_BIT 0x0040
#define GL_MAP_COHERENT_BIT 0x0080
#define GL_TEXTURE_MAX_ANISOTROPY 0x84FE

#else

#define GL_TRUE 1
#define GL_FALSE 0

#define GL_R8 0x8229
#define GL_R16F 0x822D
#define GL_R32F 0x822E
#define GL_R8I 0x8231
#define GL_R16I 0x8233
#define GL_R32I 0x8235
#define GL_R8UI 0x8232
#define GL_R16UI 0x8234
#define GL_R32UI 0x8236
#define GL_RG8 0x822B
#define GL_RG16F 0x822F
#define GL_RG32F 0x8230
#define GL_RG8I 0x8237
#define GL_RG16I 0x8239
#define GL_RG32I 0x823B
#define GL_RG8UI 0x8238
#define GL_RG16UI 0x823A
#define GL_RG32UI 0x823C
#define GL_RGB16F 0x881B
#define GL_RGB32F 0x8815
#define GL_RGB32I 0x8D83
#define GL_RGB32UI 0x8D71
#define GL_RGBA8 0x8058
#define GL_RGBA16F 0x881A
#define GL_RGBA32F 0x8814
#define GL_RGBA8I 0x8D8E
#define GL_RGBA16I 0x8D88
#define GL_RGBA32I 0x8D82
#define GL_RGBA8UI 0x8D7C
#define GL_RGBA16UI 0x8D76
#define GL_RGBA32UI 0x8D70

#define GL_RGB8 0x8051
#define GL_SRGB8 0x8C41
#define GL_SRGB8_ALPHA8 0x8C43
#define GL_DEPTH_COMPONENT16 0x81A5
#define GL_DEPTH_COMPONENT24 0x81A6
#define GL_DEPTH_COMPONENT32 0x81A7

#define GL_TEXTURE_BUFFER 0x8C2A
#define GL_MAP_WRITE_BIT 0x0002
#define GL_MAP_PERSISTENT_BIT 0x0040
#define GL_MAP_COHERENT_BIT 0x0080

#define GL_FRAMEBUFFER 0x8D40
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_COLOR_ATTACHMENT0 0x8CE0

#define GL_TEXTURE_2D_ARRAY 0x8C1A
#define GL_TEXTURE_CUBE_MAP_ARRAY 0x9009
#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_BGR 0x80E0
#define GL_BGRA 0x80E1
#define GL_UNSIGNED_BYTE 0x1401
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_REPEAT 0x2901
#define GL_TEXTURE_MAX_ANISOTROPY 0x84FE

#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_VERTEX_SHADER_BIT 0x00000001
#define GL_FRAGMENT_SHADER_BIT 0x00000002

#define GL_CULL_FACE 0x0B44
#define GL_BLEND 0x0BE2
#define GL_DEPTH_TEST 0x0B71
#define GL_FRAMEBUFFER_SRGB 0x8DB9
#define GL_TEXTURE_CUBE_MAP_SEAMLESS 0x884F

#define GL_TRIANGLES 0x0004

#define GL_DEBUG_SOURCE_API 0x8246
#define GL_DEBUG_SEVERITY_NOTIFICATION 0x826B
#define GL_DEBUG_SEVERITY_HIGH 0x9146
#define GL_DEBUG_TYPE_ERROR 0x824C

void (*glClear)(u32 mask);
void (*glFinish)();
void (*glEnable)(u32 cap);
void (*glDisable)(u32 cap);
void (*glViewport)(i32 x, i32 y, i32 width, i32 height);
void (*glCreateBuffers)(i32 n, u32 * buffers);
void (*glCreateTextures)(u32 target, i32 n, u32 * textures);
void (*glCreateSamplers)(i32 n, u32 * samplers);
void (*glCreateFramebuffers)(i32 n, u32 * framebuffers);
u32 (*glCreateShaderProgramv)(u32 type, i32 count, const char ** strings);
void (*glCreateProgramPipelines)(i32 n, u32 * pipelines);
void (*glDeleteBuffers)(i32 n, const u32 * buffers);
void (*glDeleteTextures)(i32 n, const u32 * textures);
void (*glDeleteSamplers)(i32 n, const u32 * samplers);
void (*glDeleteFramebuffers)(i32 n, const u32 * framebuffers);
void (*glDeleteProgram)(u32 program);
void (*glDeleteProgramPipelines)(i32 n, const u32 * pipelines);
void (*glBindTextures)(u32 first, i32 count, const u32 * textures);
void (*glBindSamplers)(u32 first, i32 count, const u32 * samplers);
void (*glBindFramebuffer)(u32 target, u32 framebuffer);
void (*glBindProgramPipeline)(u32 pipeline);
void (*glNamedBufferStorage)(u32 buffer, ptrdiff_t size, const void * data, u32 flags);
void * (*glMapNamedBufferRange)(u32 buffer, ptrdiff_t offset, ptrdiff_t length, u32 access);
void (*glTextureBuffer)(u32 texture, u32 internalformat, u32 buffer);
void (*glTextureStorage3D)(u32 texture, i32 levels, u32 internalformat, i32 width, i32 height, i32 depth);
void (*glTextureSubImage3D)(u32 texture, i32 level, i32 xoffset, i32 yoffset, i32 zoffset, i32 width, i32 height, i32 depth, u32 format, u32 type, const void * pixels);
void (*glGetTextureSubImage)(u32 texture, i32 level, i32 xoffset, i32 yoffset, i32 zoffset, i32 width, i32 height, i32 depth, u32 format, u32 type, i32 bufSize, void * pixels);
void (*glGenerateTextureMipmap)(u32 texture);
void (*glSamplerParameteri)(u32 sampler, u32 pname, i32 param);
void (*glNamedFramebufferTextureLayer)(u32 framebuffer, u32 attachment, u32 texture, i32 level, i32 layer);
void (*glNamedFramebufferDrawBuffers)(u32 framebuffer, i32 n, const u32 * bufs);
void (*glUseProgramStages)(u32 pipeline, u32 stages, u32 program);
void (*glDrawArraysInstancedBaseInstance)(u32 mode, i32 first, i32 count, i32 instancecount, u32 baseinstance);
void (*glProgramUniform1uiv)(u32 program, i32 location, i32 count, const u32 * value);
void (*glProgramUniform1iv)(u32 program, i32 location, i32 count, const i32 * value);
void (*glProgramUniform1dv)(u32 program, i32 location, i32 count, const f64 * value);
void (*glProgramUniform1fv)(u32 program, i32 location, i32 count, const f32 * value);
void (*glProgramUniform2fv)(u32 program, i32 location, i32 count, const f32 * value);
void (*glProgramUniform3fv)(u32 program, i32 location, i32 count, const f32 * value);
void (*glProgramUniform4fv)(u32 program, i32 location, i32 count, const f32 * value);

typedef void (*GLCLEARPROC)(u32 mask);
typedef void (*GLFINISHPROC)();
typedef void (*GLENABLEPROC)(u32 cap);
typedef void (*GLDISABLEPROC)(u32 cap);
typedef void (*GLVIEWPORTPROC)(i32 x, i32 y, i32 width, i32 height);
typedef void (*GLCREATEBUFFERSPROC)(i32 n, u32 * buffers);
typedef void (*GLCREATETEXTURESPROC)(u32 target, i32 n, u32 * textures);
typedef void (*GLCREATESAMPLERSPROC)(i32 n, u32 * samplers);
typedef void (*GLCREATEFRAMEBUFFERSPROC)(i32 n, u32 * framebuffers);
typedef u32 (*GLCREATESHADERPROGRAMVPROC)(u32 type, i32 count, const char ** strings);
typedef void (*GLCREATEPROGRAMPIPELINESPROC)(i32 n, u32 * pipelines);
typedef void (*GLDELETEBUFFERSPROC)(i32 n, const u32 * buffers);
typedef void (*GLDELETETEXTURESPROC)(i32 n, const u32 * textures);
typedef void (*GLDELETESAMPLERSPROC)(i32 n, const u32 * samplers);
typedef void (*GLDELETEFRAMEBUFFERSPROC)(i32 n, const u32 * framebuffers);
typedef void (*GLDELETEPROGRAMPROC)(u32 program);
typedef void (*GLDELETEPROGRAMPIPELINESPROC)(i32 n, const u32 * pipelines);
typedef void (*GLBINDTEXTURESPROC)(u32 first, i32 count, const u32 * textures);
typedef void (*GLBINDSAMPLERSPROC)(u32 first, i32 count, const u32 * samplers);
typedef void (*GLBINDFRAMEBUFFERPROC)(u32 target, u32 framebuffer);
typedef void (*GLBINDPROGRAMPIPELINEPROC)(u32 pipeline);
typedef void (*GLNAMEDBUFFERSTORAGEPROC)(u32 buffer, ptrdiff_t size, const void * data, u32 flags);
typedef void * (*GLMAPNAMEDBUFFERRANGEPROC)(u32 buffer, ptrdiff_t offset, ptrdiff_t length, u32 access);
typedef void (*GLTEXTUREBUFFERPROC)(u32 texture, u32 internalformat, u32 buffer);
typedef void (*GLTEXTURESTORAGE3DPROC)(u32 texture, i32 levels, u32 internalformat, i32 width, i32 height, i32 depth);
typedef void (*GLTEXTURESUBIMAGE3DPROC)(u32 texture, i32 level, i32 xoffset, i32 yoffset, i32 zoffset, i32 width, i32 height, i32 depth, u32 format, u32 type, const void * pixels);
typedef void (*GLGETTEXTURESUBIMAGEPROC)(u32 texture, i32 level, i32 xoffset, i32 yoffset, i32 zoffset, i32 width, i32 height, i32 depth, u32 format, u32 type, i32 bufSize, void * pixels);
typedef void (*GLGENERATETEXTUREMIPMAPPROC)(u32 texture);
typedef void (*GLSAMPLERPARAMETERIPROC)(u32 sampler, u32 pname, i32 param);
typedef void (*GLNAMEDFRAMEBUFFERTEXTURELAYERPROC)(u32 framebuffer, u32 attachment, u32 texture, i32 level, i32 layer);
typedef void (*GLNAMEDFRAMEBUFFERDRAWBUFFERSPROC)(u32 framebuffer, i32 n, const u32 * bufs);
typedef void (*GLUSEPROGRAMSTAGESPROC)(u32 pipeline, u32 stages, u32 program);
typedef void (*GLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)(u32 mode, i32 first, i32 count, i32 instancecount, u32 baseinstance);
typedef void (*GLPROGRAMUNIFORM1UIVPROC)(u32 program, i32 location, i32 count, const u32 * value);
typedef void (*GLPROGRAMUNIFORM1IVPROC)(u32 program, i32 location, i32 count, const i32 * value);
typedef void (*GLPROGRAMUNIFORM1DVPROC)(u32 program, i32 location, i32 count, const f64 * value);
typedef void (*GLPROGRAMUNIFORM1FVPROC)(u32 program, i32 location, i32 count, const f32 * value);
typedef void (*GLPROGRAMUNIFORM2FVPROC)(u32 program, i32 location, i32 count, const f32 * value);
typedef void (*GLPROGRAMUNIFORM3FVPROC)(u32 program, i32 location, i32 count, const f32 * value);
typedef void (*GLPROGRAMUNIFORM4FVPROC)(u32 program, i32 location, i32 count, const f32 * value);

#endif

// HELPERS /////////////////////////////////////////////////////////////////////

#define x_b8      GL_R8
#define x_f16     GL_R16F
#define x_f32     GL_R32F
#define x_i8      GL_R8I
#define x_i16     GL_R16I
#define x_i32     GL_R32I
#define x_u8      GL_R8UI
#define x_u16     GL_R16UI
#define x_u32     GL_R32UI
#define xy_b8     GL_RG8
#define xy_f16    GL_RG16F
#define xy_f32    GL_RG32F
#define xy_i8     GL_RG8I
#define xy_i16    GL_RG16I
#define xy_i32    GL_RG32I
#define xy_u8     GL_RG8UI
#define xy_u16    GL_RG16UI
#define xy_u32    GL_RG32UI
#define xyz_f32   GL_RGB32F
#define xyz_i32   GL_RGB32I
#define xyz_u32   GL_RGB32UI
#define xyzw_b8   GL_RGBA8
#define xyzw_f16  GL_RGBA16F
#define xyzw_f32  GL_RGBA32F
#define xyzw_i8   GL_RGBA8I
#define xyzw_i16  GL_RGBA16I
#define xyzw_i32  GL_RGBA32I
#define xyzw_u8   GL_RGBA8UI
#define xyzw_u16  GL_RGBA16UI
#define xyzw_u32  GL_RGBA32UI

#define rgb_b8    GL_RGB8
#define rgba_b8   GL_RGBA8
#define srgb_b8   GL_SRGB8
#define srgba_b8  GL_SRGB8_ALPHA8
#define rgba_f16  GL_RGBA16F
#define rgba_f32  GL_RGBA32F
#define depth_b16 GL_DEPTH_COMPONENT16
#define depth_b24 GL_DEPTH_COMPONENT24
#define depth_b32 GL_DEPTH_COMPONENT32

#define VERT_HEAD \
  "#version 330\n" \
  "#extension GL_ARB_shader_precision          : enable\n" \
  "#extension GL_ARB_shading_language_420pack  : enable\n" \
  "#extension GL_ARB_explicit_uniform_location : enable\n" \
  "out gl_PerVertex { vec4 gl_Position; };\n"

#define FRAG_HEAD \
  "#version 330\n" \
  "#extension GL_ARB_shader_precision          : enable\n" \
  "#extension GL_ARB_conservative_depth        : enable\n" \
  "#extension GL_ARB_texture_cube_map_array    : enable\n" \
  "#extension GL_ARB_shading_language_420pack  : enable\n" \
  "#extension GL_ARB_shading_language_packing  : enable\n" \
  "#extension GL_ARB_explicit_uniform_location : enable\n" \
  "layout(depth_unchanged) out float gl_FragDepth;\n"

// TYPES ///////////////////////////////////////////////////////////////////////

typedef struct
{
  union
  {
    u8   * ptr;
    i8   * as_i8;
    u8   * as_u8;
    i16  * as_i16;
    u16  * as_u16;
    i32  * as_i32;
    u32  * as_u32;
    f32  * as_f32;
    struct
    {
      union { f32 x; f32 u; };
      union { f32 y; f32 v; };
    } * as_vec2;
    struct
    {
      union { f32 x; f32 r; };
      union { f32 y; f32 g; };
      union { f32 z; f32 b; };
    } * as_vec3;
    struct
    {
      union { f32 x; f32 r; };
      union { f32 y; f32 g; };
      union { f32 z; f32 b; };
      union { f32 w; f32 a; };
    } * as_vec4;
  };
  u32 bytes;
  u32 count;
  u32 format;
  u32 buffer_id;
  u32 id;
} gpu_storage_t;

typedef struct
{
  i32 w;
  i32 h;
  i32 count;
  u32 format;
  i32 mipmap;
  u8  cubemap;
  u32 id;
} gpu_texture_t;

typedef struct
{
  i32 aniso;
  i32 min;
  i32 mag;
  i32 wrap;
  u32 id;
} gpu_sampler_t;

typedef struct
{
  u32 count;
  u32 instance_count;
  u32 first;
  u32 instance_first;
} gpu_cmd_t;

// PROTOTYPES //////////////////////////////////////////////////////////////////

#ifndef __cplusplus
#define gfStorageCreate(...) gfStorageCreateFromStruct((gpu_storage_t){__VA_ARGS__})
#define gfTextureCreate(...) gfTextureCreateFromStruct((gpu_texture_t){__VA_ARGS__})
#define gfSamplerCreate(...) gfSamplerCreateFromStruct((gpu_sampler_t){__VA_ARGS__})
#endif

gpu_storage_t gfStorageCreateFromStruct(gpu_storage_t tbo);
gpu_texture_t gfTextureCreateFromStruct(gpu_texture_t texture);
gpu_sampler_t gfSamplerCreateFromStruct(gpu_sampler_t sampler);
gpu_texture_t gfTextureCreateFromBmp(i32 width, i32 height, i32 mipmap, i32 texture_count, const char ** texture_paths);
gpu_texture_t gfCubemapCreateFromBmp(i32 width, i32 height, i32 mipmap, i32 texture_count, const char ** pos_x_texture_paths, const char ** neg_x_texture_paths, const char ** pos_y_texture_paths, const char ** neg_y_texture_paths, const char ** pos_z_texture_paths, const char ** neg_z_texture_paths);
void gfTextureSetPixels(u32 texture_id, i32 texture_layer, i32 x, i32 y, i32 width, i32 height, u32 format, const void * data);
void gfTextureGetPixels(u32 texture_id, i32 texture_layer, i32 x, i32 y, i32 width, i32 height, u32 format, i32 pixels_bytes, void * pixels);
void gfTextureSaveToBmp(u32 texture_id, i32 texture_layer, i32 width, i32 height, const char * bmp_filepath);
u32 gfFboCreate(u32 depth_texture_id, i32 depth_texture_layer, i32 color_texture_count, const u32 * color_texture_ids, const i32 * color_texture_layers);
void gfFboBind(u32 fbo_id);
u32 gfProgramCreateFromFile(u32 shader_type, const char * shader_filepath);
u32 gfProgramCreateFromString(u32 shader_type, const char * shader_string);
u32 gfProgramPipelineCreate(u32 vert_program, u32 frag_program);
void gfClear();
void gfDraw(u32 program_pipeline, u32 gpu_cmd_count, const gpu_cmd_t * gpu_cmd);
void gfFire(u32 program_pipeline, u32 count);
void gfDebugCallback(u32 source, u32 type, u32 id, u32 severity, i32 length, const char * message, void * userdata);
void gfCheckExtensions(i32 extensions_count, const char ** extensions);
void gfWindow(SDL_Window ** sdl_window, SDL_GLContext * sdl_glcontext, u32 sdl_init_flags, u32 sdl_window_flags, const char * window_title, i32 window_width, i32 window_height, i32 msaa_samples);
void gfError(const char * title, const char * description);

#ifdef __cplusplus
}
#endif
#endif // GLFAST_H_

// IMPLEMENTATION //////////////////////////////////////////////////////////////

#ifdef GLFAST_IMPLEMENTATION

gpu_storage_t gfStorageCreateFromStruct(gpu_storage_t tbo)
{
  if(!tbo.format)
    gfError(__FUNCTION__, "Error: TBO format is 0");

  if(!tbo.bytes && !tbo.count)
    gfError(__FUNCTION__, "Error: TBO bytes and count are 0");

  u32 elem_width = 0;
  u32 elem_bytes = 0;

  switch(tbo.format)
  {
    case(x_b8):     elem_width = 1; elem_bytes = sizeof(u8);  break;
    case(x_f16):    elem_width = 1; elem_bytes = sizeof(f16); break;
    case(x_f32):    elem_width = 1; elem_bytes = sizeof(f32); break;
    case(x_i8):     elem_width = 1; elem_bytes = sizeof(i8);  break;
    case(x_i16):    elem_width = 1; elem_bytes = sizeof(i16); break;
    case(x_i32):    elem_width = 1; elem_bytes = sizeof(i32); break;
    case(x_u8):     elem_width = 1; elem_bytes = sizeof(u8);  break;
    case(x_u16):    elem_width = 1; elem_bytes = sizeof(u16); break;
    case(x_u32):    elem_width = 1; elem_bytes = sizeof(u32); break;
    case(xy_b8):    elem_width = 2; elem_bytes = sizeof(u8);  break;
    case(xy_f16):   elem_width = 2; elem_bytes = sizeof(f16); break;
    case(xy_f32):   elem_width = 2; elem_bytes = sizeof(f32); break;
    case(xy_i8):    elem_width = 2; elem_bytes = sizeof(i8);  break;
    case(xy_i16):   elem_width = 2; elem_bytes = sizeof(i16); break;
    case(xy_i32):   elem_width = 2; elem_bytes = sizeof(i32); break;
    case(xy_u8):    elem_width = 2; elem_bytes = sizeof(u8);  break;
    case(xy_u16):   elem_width = 2; elem_bytes = sizeof(u16); break;
    case(xy_u32):   elem_width = 2; elem_bytes = sizeof(u32); break;
    case(xyz_f32):  elem_width = 3; elem_bytes = sizeof(f32); break;
    case(xyz_i32):  elem_width = 3; elem_bytes = sizeof(i32); break;
    case(xyz_u32):  elem_width = 3; elem_bytes = sizeof(u32); break;
    case(xyzw_b8):  elem_width = 4; elem_bytes = sizeof(u8);  break;
    case(xyzw_f16): elem_width = 4; elem_bytes = sizeof(f16); break;
    case(xyzw_f32): elem_width = 4; elem_bytes = sizeof(f32); break;
    case(xyzw_i8):  elem_width = 4; elem_bytes = sizeof(i8);  break;
    case(xyzw_i16): elem_width = 4; elem_bytes = sizeof(i16); break;
    case(xyzw_i32): elem_width = 4; elem_bytes = sizeof(i32); break;
    case(xyzw_u8):  elem_width = 4; elem_bytes = sizeof(u8);  break;
    case(xyzw_u16): elem_width = 4; elem_bytes = sizeof(u16); break;
    case(xyzw_u32): elem_width = 4; elem_bytes = sizeof(u32); break;
    default: gfError(__FUNCTION__, "Error: Wrong TBO format");
  }
  
  if(!tbo.bytes)
    tbo.bytes = tbo.count * elem_width * elem_bytes;
  else
    tbo.count = tbo.bytes / elem_width / elem_bytes;
  
  glCreateBuffers(1, &tbo.buffer_id);
  glCreateTextures(GL_TEXTURE_BUFFER, 1, &tbo.id);
  
  u32 flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
  glNamedBufferStorage(tbo.buffer_id, tbo.bytes, NULL, flags);
  tbo.ptr = (u8 *)glMapNamedBufferRange(tbo.buffer_id, 0, tbo.bytes, flags);
  
  glTextureBuffer(tbo.id, tbo.format, tbo.buffer_id);
  
  return tbo;
}

gpu_texture_t gfTextureCreateFromStruct(gpu_texture_t texture)
{
  if(!texture.w)
    gfError(__FUNCTION__, "Error: Texture w is 0");
  
  if(!texture.h)
    gfError(__FUNCTION__, "Error: Texture h is 0");
  
  if(!texture.format)
    gfError(__FUNCTION__, "Error: Texture format is 0");
  
  if(!texture.count)  texture.count  = 1;
  if(!texture.mipmap) texture.mipmap = 1;

  glCreateTextures(texture.cubemap ? GL_TEXTURE_CUBE_MAP_ARRAY : GL_TEXTURE_2D_ARRAY, 1, &texture.id);
  glTextureStorage3D(texture.id, texture.mipmap, texture.format, texture.w, texture.h, texture.count * (texture.cubemap ? 6 : 1));
  
  return texture;
}

gpu_texture_t gfTextureCreateFromBmp(
  i32 width,
  i32 height,
  i32 mipmap,
  i32 texture_count,
  const char ** texture_paths)
{
  gpu_texture_t texture;
  
  texture.w       = width;
  texture.h       = height;
  texture.count   = texture_count;
  texture.format  = srgb_b8;
  texture.mipmap  = mipmap;
  texture.cubemap = 0;
  
  texture = gfTextureCreateFromStruct(texture);
  
  for(i32 i = 0; i < texture_count; ++i)
  {
    SDL_Surface * bmp = SDL_LoadBMP(texture_paths[i]);
    if(!bmp) gfError("Error: File not found", texture_paths[i]);
    gfTextureSetPixels(texture.id, i, 0, 0, texture.w, texture.h, GL_BGR, bmp->pixels);
    SDL_FreeSurface(bmp);
  }

  glGenerateTextureMipmap(texture.id);

  return texture;
}

gpu_texture_t gfCubemapCreateFromBmp(
  i32 width,
  i32 height,
  i32 mipmap,
  i32 texture_count,
  const char ** pos_x_texture_paths,
  const char ** neg_x_texture_paths,
  const char ** pos_y_texture_paths,
  const char ** neg_y_texture_paths,
  const char ** pos_z_texture_paths,
  const char ** neg_z_texture_paths)
{
  gpu_texture_t texture;
  
  texture.w       = width;
  texture.h       = height;
  texture.count   = texture_count;
  texture.format  = srgb_b8;
  texture.mipmap  = mipmap;
  texture.cubemap = 1;
  
  texture = gfTextureCreateFromStruct(texture);
  
  for(i32 i = 0; i < texture_count; ++i)
  {
    SDL_Surface * bmp_pos_x = SDL_LoadBMP(pos_x_texture_paths[i]);
    SDL_Surface * bmp_neg_x = SDL_LoadBMP(neg_x_texture_paths[i]);
    SDL_Surface * bmp_pos_y = SDL_LoadBMP(pos_y_texture_paths[i]);
    SDL_Surface * bmp_neg_y = SDL_LoadBMP(neg_y_texture_paths[i]);
    SDL_Surface * bmp_pos_z = SDL_LoadBMP(pos_z_texture_paths[i]);
    SDL_Surface * bmp_neg_z = SDL_LoadBMP(neg_z_texture_paths[i]);

    if(!bmp_pos_x) gfError("Error: File not found", pos_x_texture_paths[i]);
    if(!bmp_neg_x) gfError("Error: File not found", neg_x_texture_paths[i]);
    if(!bmp_pos_y) gfError("Error: File not found", pos_y_texture_paths[i]);
    if(!bmp_neg_y) gfError("Error: File not found", neg_y_texture_paths[i]);
    if(!bmp_pos_z) gfError("Error: File not found", pos_z_texture_paths[i]);
    if(!bmp_neg_z) gfError("Error: File not found", neg_z_texture_paths[i]);
    
    gfTextureSetPixels(texture.id, i * 6 + 0, 0, 0, texture.w, texture.h, GL_BGR, bmp_pos_x->pixels);
    gfTextureSetPixels(texture.id, i * 6 + 1, 0, 0, texture.w, texture.h, GL_BGR, bmp_neg_x->pixels);
    gfTextureSetPixels(texture.id, i * 6 + 2, 0, 0, texture.w, texture.h, GL_BGR, bmp_pos_y->pixels);
    gfTextureSetPixels(texture.id, i * 6 + 3, 0, 0, texture.w, texture.h, GL_BGR, bmp_neg_y->pixels);
    gfTextureSetPixels(texture.id, i * 6 + 4, 0, 0, texture.w, texture.h, GL_BGR, bmp_pos_z->pixels);
    gfTextureSetPixels(texture.id, i * 6 + 5, 0, 0, texture.w, texture.h, GL_BGR, bmp_neg_z->pixels);
    
    SDL_FreeSurface(bmp_pos_x);
    SDL_FreeSurface(bmp_neg_x);
    SDL_FreeSurface(bmp_pos_y);
    SDL_FreeSurface(bmp_neg_y);
    SDL_FreeSurface(bmp_pos_z);
    SDL_FreeSurface(bmp_neg_z);
  }

  glGenerateTextureMipmap(texture.id);

  return texture;
}

gpu_sampler_t gfSamplerCreateFromStruct(gpu_sampler_t sampler)
{
  if(!sampler.aniso) sampler.aniso = 1;
  if(!sampler.min)   sampler.min   = GL_LINEAR_MIPMAP_LINEAR;
  if(!sampler.mag)   sampler.mag   = GL_LINEAR;
  if(!sampler.wrap)  sampler.wrap  = GL_REPEAT;

  glCreateSamplers(1, &sampler.id);
  
  glSamplerParameteri(sampler.id, GL_TEXTURE_MAX_ANISOTROPY, sampler.aniso);
  glSamplerParameteri(sampler.id, GL_TEXTURE_MIN_FILTER, sampler.min);
  glSamplerParameteri(sampler.id, GL_TEXTURE_MAG_FILTER, sampler.mag);
  glSamplerParameteri(sampler.id, GL_TEXTURE_WRAP_S, sampler.wrap);
  glSamplerParameteri(sampler.id, GL_TEXTURE_WRAP_T, sampler.wrap);

  return sampler;
}

void gfTextureSetPixels(
  u32 texture_id,
  i32 texture_layer,
  i32 x,
  i32 y,
  i32 width,
  i32 height,
  u32 format,
  const void * pixels)
{
  glTextureSubImage3D(texture_id, 0, x, y, texture_layer, width, height, 1, format, GL_UNSIGNED_BYTE, pixels);
}

void gfTextureGetPixels(
  u32 texture_id,
  i32 texture_layer,
  i32 x,
  i32 y,
  i32 width,
  i32 height,
  u32 format,
  i32 pixels_bytes,
  void * pixels)
{
  glGetTextureSubImage(texture_id, 0, x, y, texture_layer, width, height, 1, format, GL_UNSIGNED_BYTE, pixels_bytes, pixels);
}

void gfTextureSaveToBmp(
  u32 texture_id,
  i32 texture_layer,
  i32 width,
  i32 height,
  const char * bmp_filepath)
{
  i32  pixels_bytes = width * height * 3;
  u8 * pixels = (u8 *)SDL_malloc((u32)pixels_bytes);
  
  gfTextureGetPixels(texture_id, texture_layer, 0, 0, width, height, GL_BGR, pixels_bytes, pixels);
  
  SDL_Surface * bmp = SDL_CreateRGBSurfaceFrom(pixels, width, height, 3 * 8, 3 * width, 0, 0, 0, 0);
  SDL_SaveBMP(bmp, bmp_filepath);
  SDL_FreeSurface(bmp);
  
  SDL_free(pixels);
}

u32 gfFboCreate(
  u32 depth_texture_id,
  i32 depth_texture_layer,
  i32 color_texture_count,
  const u32 * color_texture_ids,
  const i32 * color_texture_layers)
{
  if(color_texture_count < 0 || color_texture_count > 8)
    gfError(__FUNCTION__, "Error: Number of color attachments must be in [0,8]");

  u32 fbo;
  glCreateFramebuffers(1, &fbo);
  
  if(depth_texture_id)
    glNamedFramebufferTextureLayer(fbo, GL_DEPTH_ATTACHMENT, depth_texture_id, 0, depth_texture_layer);
  
  if(color_texture_count)
  {
    u32 attachments[8] = {0};
    
    for(i32 i = 0; i < color_texture_count; ++i)
    {
      attachments[i] = GL_COLOR_ATTACHMENT0 + (u32)i;
      glNamedFramebufferTextureLayer(fbo, GL_COLOR_ATTACHMENT0 + (u32)i, color_texture_ids[i], 0, color_texture_layers ? color_texture_layers[i] : 0);
    }
    
    glNamedFramebufferDrawBuffers(fbo, color_texture_count, attachments);
  }
  
  return fbo;
}

void gfFboBind(u32 fbo_id)
{
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
}

u32 gfProgramCreateFromFile(
  u32 shader_type,
  const char * shader_filepath)
{
  SDL_RWops * fd = SDL_RWFromFile(shader_filepath, "rb");
  if(fd == NULL) gfError("Error: File not found", shader_filepath);
  SDL_RWseek(fd, 0, RW_SEEK_END);
  u32 bytes = (u32)SDL_RWtell(fd);
  SDL_RWseek(fd, 0, RW_SEEK_SET);
  char * src = (char *)SDL_malloc(bytes + 1);
  src[bytes] = 0;
  SDL_RWread(fd, src, bytes, 1);
  SDL_RWclose(fd);
  char * shader_string = &src[0];
  
  uint32_t program = glCreateShaderProgramv(shader_type, 1, (const char **)&shader_string);

  SDL_free(src);
  return program;
}

u32 gfProgramCreateFromString(
  u32 shader_type,
  const char * shader_string)
{
  return glCreateShaderProgramv(shader_type, 1, (const char **)&shader_string);
}

u32 gfProgramPipelineCreate(
  u32 vert_program,
  u32 frag_program)
{
  u32 ppo;
  glCreateProgramPipelines(1, &ppo);
  
  if(vert_program) glUseProgramStages(ppo, GL_VERTEX_SHADER_BIT, vert_program);
  if(frag_program) glUseProgramStages(ppo, GL_FRAGMENT_SHADER_BIT, frag_program);
  
  return ppo;
}

void gfClear()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void gfDraw(
  u32 program_pipeline,
  u32 gpu_cmd_count,
  const gpu_cmd_t * gpu_cmd)
{
  glBindProgramPipeline(program_pipeline);
  
  for(u32 i = 0; i < gpu_cmd_count; ++i)
  {
    gpu_cmd_t cmd = gpu_cmd[i];
    glDrawArraysInstancedBaseInstance(GL_TRIANGLES, (i32)cmd.first, (i32)cmd.count, (i32)cmd.instance_count, cmd.instance_first);
  }
  
  glBindProgramPipeline(0);
}

void gfFire(
  u32 program_pipeline,
  u32 count)
{
  gpu_cmd_t cmd;
  cmd.first = 0;
  cmd.count = count;
  cmd.instance_first = 0;
  cmd.instance_count = 1;
  gfDraw(program_pipeline, 1, &cmd);
}

void gfDebugCallback(
  u32 source,
  u32 type,
  u32 id,
  u32 severity,
  i32 length,
  const char * message,
  void * userdata)
{
  const char * GL_ERROR_SOURCE[] =
  {
    "API",
    "WINDOW SYSTEM",
    "SHADER COMPILER",
    "THIRD PARTY",
    "APPLICATION",
    "OTHER"
  };
  
  const char * GL_ERROR_SEVERITY[] =
  {
    "HIGH",
    "MEDIUM",
    "LOW",
    "NOTIFICATION"
  };
  
  const char * GL_ERROR_TYPE[] =
  {
    "ERROR",
    "DEPRECATED BEHAVIOR",
    "UNDEFINED DEHAVIOUR",
    "PORTABILITY",
    "PERFORMANCE",
    "OTHER"
  };

  SDL_Log(
    "[ID: %u]"       "\n"
    "[SOURCE: %s]"   "\n"
    "[SEVERITY: %s]" "\n"
    "[TYPE: %s]"     "\n"
    "[MESSAGE: %s]"  "\n",
    id,
    GL_ERROR_SOURCE[source - GL_DEBUG_SOURCE_API],
    GL_ERROR_SEVERITY[
      severity != GL_DEBUG_SEVERITY_NOTIFICATION ?
      severity  - GL_DEBUG_SEVERITY_HIGH : 3
    ],
    GL_ERROR_TYPE[type - GL_DEBUG_TYPE_ERROR],
    message
  );
}

void gfCheckExtensions(
  i32 extensions_count,
  const char ** extensions)
{
  for(i32 i = 0; i < extensions_count; ++i)
  {
    if(!SDL_GL_ExtensionSupported(extensions[i]))
      gfError("Error: Unsupported OpenGL Extension", extensions[i]);
  }
}

void gfWindow(
  SDL_Window ** sdl_window,
  SDL_GLContext * sdl_glcontext,
  u32 sdl_init_flags,
  u32 sdl_window_flags,
  const char * window_title,
  i32 window_width,
  i32 window_height,
  i32 msaa_samples)
{
  if(SDL_Init(SDL_INIT_VIDEO | sdl_init_flags))
    gfError("Error: SDL_Init", SDL_GetError());

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, msaa_samples);
  SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
  
  #ifndef RELEASE
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
  #endif
  
  *sdl_window = SDL_CreateWindow(
    window_title,
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    window_width, window_height, SDL_WINDOW_OPENGL | sdl_window_flags
  );

  if(*sdl_window == NULL)
    gfError("Error: SDL_CreateWindow", SDL_GetError());

  *sdl_glcontext = SDL_GL_CreateContext(*sdl_window);

  if(*sdl_glcontext == NULL)
    gfError("Error: SDL_GL_CreateContext", SDL_GetError());

  #ifdef GLFAST_EXTERNAL_LOADER

    //SDL_GL_MakeCurrent(sdl_window, sdl_glcontext);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    // bool err = false;
    // bool err = gl3wInit() != 0;
    // bool err = glewInit() != GLEW_OK;
    bool err = gladLoadGL() == 0;
    if (err)
      gfError(__FUNCTION__, "Failed to initialize OpenGL loader");

  #else

  if(SDL_GL_LoadLibrary(NULL))
    gfError("Error: SDL_GL_LoadLibrary", SDL_GetError());

  glClear = (GLCLEARPROC)SDL_GL_GetProcAddress("glClear");
  glFinish = (GLFINISHPROC)SDL_GL_GetProcAddress("glFinish");
  glEnable = (GLENABLEPROC)SDL_GL_GetProcAddress("glEnable");
  glDisable = (GLDISABLEPROC)SDL_GL_GetProcAddress("glDisable");
  glViewport = (GLVIEWPORTPROC)SDL_GL_GetProcAddress("glViewport");
  glCreateBuffers = (GLCREATEBUFFERSPROC)SDL_GL_GetProcAddress("glCreateBuffers");
  glCreateTextures = (GLCREATETEXTURESPROC)SDL_GL_GetProcAddress("glCreateTextures");
  glCreateSamplers = (GLCREATESAMPLERSPROC)SDL_GL_GetProcAddress("glCreateSamplers");
  glCreateFramebuffers = (GLCREATEFRAMEBUFFERSPROC)SDL_GL_GetProcAddress("glCreateFramebuffers");
  glCreateShaderProgramv = (GLCREATESHADERPROGRAMVPROC)SDL_GL_GetProcAddress("glCreateShaderProgramv");
  glCreateProgramPipelines = (GLCREATEPROGRAMPIPELINESPROC)SDL_GL_GetProcAddress("glCreateProgramPipelines");
  glDeleteBuffers = (GLDELETEBUFFERSPROC)SDL_GL_GetProcAddress("glDeleteBuffers");
  glDeleteTextures = (GLDELETETEXTURESPROC)SDL_GL_GetProcAddress("glDeleteTextures");
  glDeleteSamplers = (GLDELETESAMPLERSPROC)SDL_GL_GetProcAddress("glDeleteSamplers");
  glDeleteFramebuffers = (GLDELETEFRAMEBUFFERSPROC)SDL_GL_GetProcAddress("glDeleteFramebuffers");
  glDeleteProgram = (GLDELETEPROGRAMPROC)SDL_GL_GetProcAddress("glDeleteProgram");
  glDeleteProgramPipelines = (GLDELETEPROGRAMPIPELINESPROC)SDL_GL_GetProcAddress("glDeleteProgramPipelines");
  glBindTextures = (GLBINDTEXTURESPROC)SDL_GL_GetProcAddress("glBindTextures");
  glBindSamplers = (GLBINDSAMPLERSPROC)SDL_GL_GetProcAddress("glBindSamplers");
  glBindFramebuffer = (GLBINDFRAMEBUFFERPROC)SDL_GL_GetProcAddress("glBindFramebuffer");
  glBindProgramPipeline = (GLBINDPROGRAMPIPELINEPROC)SDL_GL_GetProcAddress("glBindProgramPipeline");
  glNamedBufferStorage = (GLNAMEDBUFFERSTORAGEPROC)SDL_GL_GetProcAddress("glNamedBufferStorage");
  glMapNamedBufferRange = (GLMAPNAMEDBUFFERRANGEPROC)SDL_GL_GetProcAddress("glMapNamedBufferRange");
  glTextureBuffer = (GLTEXTUREBUFFERPROC)SDL_GL_GetProcAddress("glTextureBuffer");
  glTextureStorage3D = (GLTEXTURESTORAGE3DPROC)SDL_GL_GetProcAddress("glTextureStorage3D");
  glTextureSubImage3D = (GLTEXTURESUBIMAGE3DPROC)SDL_GL_GetProcAddress("glTextureSubImage3D");
  glGetTextureSubImage = (GLGETTEXTURESUBIMAGEPROC)SDL_GL_GetProcAddress("glGetTextureSubImage");
  glGenerateTextureMipmap = (GLGENERATETEXTUREMIPMAPPROC)SDL_GL_GetProcAddress("glGenerateTextureMipmap");
  glSamplerParameteri = (GLSAMPLERPARAMETERIPROC)SDL_GL_GetProcAddress("glSamplerParameteri");
  glNamedFramebufferTextureLayer = (GLNAMEDFRAMEBUFFERTEXTURELAYERPROC)SDL_GL_GetProcAddress("glNamedFramebufferTextureLayer");
  glNamedFramebufferDrawBuffers = (GLNAMEDFRAMEBUFFERDRAWBUFFERSPROC)SDL_GL_GetProcAddress("glNamedFramebufferDrawBuffers");
  glUseProgramStages = (GLUSEPROGRAMSTAGESPROC)SDL_GL_GetProcAddress("glUseProgramStages");
  glDrawArraysInstancedBaseInstance = (GLDRAWARRAYSINSTANCEDBASEINSTANCEPROC)SDL_GL_GetProcAddress("glDrawArraysInstancedBaseInstance");
  glProgramUniform1uiv = (GLPROGRAMUNIFORM1UIVPROC)SDL_GL_GetProcAddress("glProgramUniform1uiv");
  glProgramUniform1iv = (GLPROGRAMUNIFORM1IVPROC)SDL_GL_GetProcAddress("glProgramUniform1iv");
  glProgramUniform1dv = (GLPROGRAMUNIFORM1DVPROC)SDL_GL_GetProcAddress("glProgramUniform1dv");
  glProgramUniform1fv = (GLPROGRAMUNIFORM1FVPROC)SDL_GL_GetProcAddress("glProgramUniform1fv");
  glProgramUniform2fv = (GLPROGRAMUNIFORM2FVPROC)SDL_GL_GetProcAddress("glProgramUniform2fv");
  glProgramUniform3fv = (GLPROGRAMUNIFORM3FVPROC)SDL_GL_GetProcAddress("glProgramUniform3fv");
  glProgramUniform4fv = (GLPROGRAMUNIFORM4FVPROC)SDL_GL_GetProcAddress("glProgramUniform4fv");
  
  typedef void (*GLCREATEVERTEXARRAYSPROC)(i32 n, u32 * arrays);
  typedef void (*GLBINDVERTEXARRAYPROC)(u32 array);
  typedef void (*GLBLENDFUNCPROC)(u32 sfactor, u32 dfactor);
  typedef void (*GLDEPTHRANGEPROC)(f64 nearVal, f64 farVal);
  
  void (*glCreateVertexArrays)(i32 n, u32 * arrays) = (GLCREATEVERTEXARRAYSPROC)SDL_GL_GetProcAddress("glCreateVertexArrays");
  void (*glBindVertexArray)(u32 array) = (GLBINDVERTEXARRAYPROC)SDL_GL_GetProcAddress("glBindVertexArray");
  void (*glBlendFunc)(u32 sfactor, u32 dfactor) = (GLBLENDFUNCPROC)SDL_GL_GetProcAddress("glBlendFunc");
  void (*glDepthRange)(f64 nearVal, f64 farVal) = (GLDEPTHRANGEPROC)SDL_GL_GetProcAddress("glDepthRange");

  #endif
  
  #ifndef RELEASE
  typedef void (*GLDEBUGPROC)(u32 source, u32 type, u32 id, u32 severity, i32 length, const char * message, const void * userParam);
  typedef void (*GLDEBUGMESSAGECALLBACKPROC)(GLDEBUGPROC callback, const void * userParam);
  void (*glDebugMessageCallback)(GLDEBUGPROC callback, const void * userParam) = (GLDEBUGMESSAGECALLBACKPROC)SDL_GL_GetProcAddress("glDebugMessageCallback");
  glDebugMessageCallback((GLDEBUGPROC)gfDebugCallback, NULL);
  #endif

  const char * extensions[] =
  {
    #ifndef RELEASE
    "GL_KHR_debug",
    #endif
    "GL_ARB_multi_bind",
    "GL_ARB_base_instance",
    "GL_ARB_buffer_storage",
    "GL_ARB_texture_storage",
    "GL_ARB_sampler_objects",
    "GL_ARB_framebuffer_sRGB",
    "GL_ARB_shader_precision",
    "GL_ARB_seamless_cube_map",
    "GL_ARB_conservative_depth",
    "GL_ARB_framebuffer_object",
    "GL_ARB_get_program_binary",
//    "GL_ARB_direct_state_access",
//    "GL_ARB_get_texture_sub_image",
    "GL_ARB_texture_cube_map_array",
    "GL_ARB_separate_shader_objects",
    "GL_ARB_shading_language_420pack",
    "GL_ARB_shading_language_packing",
    "GL_ARB_explicit_uniform_location",
    "GL_EXT_texture_filter_anisotropic",
    "GL_ARB_texture_buffer_object_rgb32"
  };
  gfCheckExtensions(sizeof(extensions) / sizeof(extensions[0]), extensions);
  
#if 0
  u32 vao;
  glCreateVertexArrays(1, &vao);
  glBindVertexArray(vao);
#endif

  glBlendFunc(0x0302, 0x0303);
  glDepthRange(-1.0, 1.0);
  
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  glEnable(GL_FRAMEBUFFER_SRGB);
}

void gfError(
  const char * title,
  const char * description)
{
  SDL_Log("%s: %s\n", title, description);
}

#endif // GLFAST_IMPLEMENTATION

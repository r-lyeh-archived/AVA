#ifndef __gl_portable_h__
#define __gl_portable_h__ 1

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned GlBool32;

#ifndef _WIN32
enum GL_PROGRAM_TARGET {
  GL_VERTEX_PROGRAM_ARB   = 0x8620,
  GL_FRAGMENT_PROGRAM_ARB = 0x8804,
};

enum GL_PROGRAM_FORMAT {
  GL_PROGRAM_FORMAT_ASCII_ARB = 0x8875,
};

void glEnable(unsigned cap);
void glDisable(unsigned cap);
void glGenProgramsARB(int n, unsigned *programs);
void glDeleteProgramsARB(int n, const unsigned *programs);
void glBindProgramARB(enum GL_PROGRAM_TARGET target, unsigned program);
void glProgramStringARB(enum GL_PROGRAM_TARGET target, enum GL_PROGRAM_FORMAT format, unsigned len, const void *string);
void glProgramEnvParameter4fARB(enum GL_PROGRAM_TARGET target, unsigned index, float x, float y, float z, float w);
void glProgramLocalParameter4fARB(enum GL_PROGRAM_TARGET target, unsigned index, float x, float y, float z, float w);

enum GL_LIST_MODE {
  GL_COMPILE = 0x1300,
};

unsigned glGenLists(int range);
void glDeleteLists(unsigned list, int range);
void glNewList(unsigned list, enum GL_LIST_MODE mode);
void glEndList(void);
void glListBase(unsigned base);
void glCallList(unsigned list);
void glCallLists(int n, unsigned type, const void *lists);

enum GL_PRIMITIVE_TOPOLOGY {
  GL_PRIMITIVE_TOPOLOGY_POINT_LIST     = 0x0000, // GL_POINTS
  GL_PRIMITIVE_TOPOLOGY_LINE_LIST      = 0x0001, // GL_LINES
  GL_PRIMITIVE_TOPOLOGY_LINE_LOOP      = 0x0002, // GL_LINE_LOOP
  GL_PRIMITIVE_TOPOLOGY_LINE_STRIP     = 0x0003, // GL_LINE_STRIP
  GL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST  = 0x0004, // GL_TRIANGLES
  GL_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP = 0x0005, // GL_TRIANGLE_STRIP
  GL_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN   = 0x0006, // GL_TRIANGLE_FAN
};

enum GL_ACTIVE_TEXTURE {
  GL_TEXTURE0 = 0x84C0,
};

void glBegin(enum GL_PRIMITIVE_TOPOLOGY topology);
void glEnd(void);
void glVertex4f(float x, float y, float z, float w);
void glNormal3f(float nx, float ny, float nz);
void glColor4f(float red, float green, float blue, float alpha);
void glMultiTexCoord4f(enum GL_ACTIVE_TEXTURE target, float s, float t, float r, float q);

enum GL_TEXTURE_TARGET {
  GL_TEXTURE_2D                  = 0x0DE1,
  GL_TEXTURE_CUBE_MAP            = 0x8513,
  GL_TEXTURE_CUBE_MAP_POSITIVE_X = 0x8515,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_X = 0x8516,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Y = 0x8517,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y = 0x8518,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Z = 0x8519,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z = 0x851A,
};

enum GL_TEXTURE_PARAMETER_NAME {
  GL_TEXTURE_MAG_FILTER     = 0x2800,
  GL_TEXTURE_MIN_FILTER     = 0x2801,
  GL_TEXTURE_WRAP_S         = 0x2802,
  GL_TEXTURE_WRAP_T         = 0x2803,
  GL_GENERATE_MIPMAP        = 0x8191,
  GL_TEXTURE_MAX_ANISOTROPY = 0x84FE,
};

enum GL_TEXTURE_FILTER {
  GL_NEAREST                = 0x2600,
  GL_LINEAR                 = 0x2601,
  GL_NEAREST_MIPMAP_NEAREST = 0x2700,
  GL_LINEAR_MIPMAP_NEAREST  = 0x2701,
  GL_NEAREST_MIPMAP_LINEAR  = 0x2702,
  GL_LINEAR_MIPMAP_LINEAR   = 0x2703,
};

enum GL_TEXTURE_ADDRESS_MODE {
  GL_TEXTURE_ADDRESS_WRAP  = 0x2901, // GL_REPEAT
  GL_TEXTURE_ADDRESS_CLAMP = 0x812F, // GL_CLAMP_TO_EDGE
};

enum GL_PIXEL_STORE_PARAMETER_NAME {
  GL_UNPACK_ALIGNMENT = 0x0CF5,
  GL_PACK_ALIGNMENT   = 0x0D05,
};

enum GL_TEXTURE_TEXEL_FORMAT {
  GL_ALPHA           = 0x1906,
  GL_RGB             = 0x1907,
  GL_RGBA            = 0x1908,
  GL_LUMINANCE       = 0x1909,
  GL_LUMINANCE_ALPHA = 0x190A,
};

enum GL_TEXTURE_TEXEL_TYPE {
  GL_BYTE                   = 0x1400,
  GL_UNSIGNED_BYTE          = 0x1401,
  GL_SHORT                  = 0x1402,
  GL_UNSIGNED_SHORT         = 0x1403,
  GL_FLOAT                  = 0x1406,
  GL_UNSIGNED_SHORT_4_4_4_4 = 0x8033,
  GL_UNSIGNED_SHORT_5_5_5_1 = 0x8034,
  GL_UNSIGNED_SHORT_5_6_5   = 0x8363,
};

void glActiveTexture(enum GL_ACTIVE_TEXTURE texture);
void glGenTextures(int n, unsigned *textures);
void glDeleteTextures(int n, const unsigned *textures);
void glBindTexture(enum GL_TEXTURE_TARGET target, unsigned texture);
void glTexParameteri(enum GL_TEXTURE_TARGET target, enum GL_TEXTURE_PARAMETER_NAME pname, int param);
void glPixelStorei(enum GL_PIXEL_STORE_PARAMETER_NAME pname, int param);
void glTexImage2D(enum GL_TEXTURE_TARGET target, int level, enum GL_TEXTURE_TEXEL_FORMAT internalformat, int width, int height, int must_be_0, enum GL_TEXTURE_TEXEL_FORMAT format, enum GL_TEXTURE_TEXEL_TYPE type, const void *pixels);
void glTexSubImage2D(enum GL_TEXTURE_TARGET target, int level, int xoffset, int yoffset, int width, int height, enum GL_TEXTURE_TEXEL_FORMAT format, enum GL_TEXTURE_TEXEL_TYPE type, const void *pixels);
void glCopyTexImage2D(enum GL_TEXTURE_TARGET target, int level, enum GL_TEXTURE_TEXEL_FORMAT internalformat, int x, int y, int width, int height, int must_be_0);
void glCopyTexSubImage2D(enum GL_TEXTURE_TARGET target, int level, int xoffset, int yoffset, int x, int y, int width, int height);
void glReadPixels(int x, int y, int width, int height, enum GL_TEXTURE_TEXEL_FORMAT format, enum GL_TEXTURE_TEXEL_TYPE type, void *pixels);

enum GL_CLEAR_MASK {
  GL_DEPTH_BUFFER_BIT   = 0x00000100,
  GL_STENCIL_BUFFER_BIT = 0x00000400,
  GL_COLOR_BUFFER_BIT   = 0x00004000,
};

void glViewport(int x, int y, int width, int height);
void glScissor(int x, int y, int width, int height);
void glClearDepthf(float depth);
void glClearStencil(int s);
void glClearColor(float red, float green, float blue, float alpha);
void glClear(enum GL_CLEAR_MASK mask);
void glFlush(void);
void glFinish(void);

enum GL_CULL_MODE {
  GL_CULL_MODE_NONE           = 0,
  GL_CULL_MODE_FRONT          = 0x0404, // GL_FRONT
  GL_CULL_MODE_BACK           = 0x0405, // GL_BACK
  GL_CULL_MODE_FRONT_AND_BACK = 0x0408, // GL_FRONT_AND_BACK
};

enum GL_FRONT_FACE {
  GL_FRONT_FACE_COUNTER_CLOCKWISE = 0x0901, // GL_CCW
  GL_FRONT_FACE_CLOCKWISE         = 0x0900, // GL_CW
};

enum GL_COMPARE_OP {
  GL_COMPARE_OP_NEVER            = 0x0200, // GL_NEVER
  GL_COMPARE_OP_LESS             = 0x0201, // GL_LESS
  GL_COMPARE_OP_EQUAL            = 0x0202, // GL_EQUAL
  GL_COMPARE_OP_LESS_OR_EQUAL    = 0x0203, // GL_LEQUAL
  GL_COMPARE_OP_GREATER          = 0x0204, // GL_GREATER
  GL_COMPARE_OP_NOT_EQUAL        = 0x0205, // GL_NOTEQUAL
  GL_COMPARE_OP_GREATER_OR_EQUAL = 0x0206, // GL_GEQUAL
  GL_COMPARE_OP_ALWAYS           = 0x0207, // GL_ALWAYS
};

enum GL_STENCIL_OP {
  GL_STENCIL_OP_KEEP                = 0x1E00, // GL_KEEP
  GL_STENCIL_OP_ZERO                = 0,      // GL_ZERO
  GL_STENCIL_OP_REPLACE             = 0x1E01, // GL_REPLACE
  GL_STENCIL_OP_INCREMENT_AND_CLAMP = 0x1E02, // GL_INCR
  GL_STENCIL_OP_DECREMENT_AND_CLAMP = 0x1E03, // GL_DECR
  GL_STENCIL_OP_INVERT              = 0x150A, // GL_INVERT
};

enum GL_LOGIC_OP {
  GL_LOGIC_OP_CLEAR         = 0x1500, // GL_CLEAR
  GL_LOGIC_OP_AND           = 0x1501, // GL_AND
  GL_LOGIC_OP_AND_REVERSE   = 0x1502, // GL_AND_REVERSE
  GL_LOGIC_OP_COPY          = 0x1503, // GL_COPY
  GL_LOGIC_OP_AND_INVERTED  = 0x1504, // GL_AND_INVERTED
  GL_LOGIC_OP_NO_OP         = 0x1505, // GL_NOOP
  GL_LOGIC_OP_XOR           = 0x1506, // GL_XOR
  GL_LOGIC_OP_OR            = 0x1507, // GL_OR
  GL_LOGIC_OP_NOR           = 0x1508, // GL_NOR
  GL_LOGIC_OP_EQUIVALENT    = 0x1509, // GL_EQUIV
  GL_LOGIC_OP_INVERT        = 0x150A, // GL_INVERT
  GL_LOGIC_OP_OR_REVERSE    = 0x150B, // GL_OR_REVERSE
  GL_LOGIC_OP_COPY_INVERTED = 0x150C, // GL_COPY_INVERTED
  GL_LOGIC_OP_OR_INVERTED   = 0x150D, // GL_OR_INVERTED
  GL_LOGIC_OP_NAND          = 0x150E, // GL_NAND
  GL_LOGIC_OP_SET           = 0x150F, // GL_SET
};

enum GL_BLEND_FACTOR {
  GL_BLEND_FACTOR_ZERO                     = 0,      // GL_ZERO
  GL_BLEND_FACTOR_ONE                      = 1,      // GL_ONE
  GL_BLEND_FACTOR_SRC_COLOR                = 0x0300, // GL_SRC_COLOR
  GL_BLEND_FACTOR_ONE_MINUS_SRC_COLOR      = 0x0301, // GL_ONE_MINUS_SRC_COLOR
  GL_BLEND_FACTOR_DST_COLOR                = 0x0306, // GL_DST_COLOR
  GL_BLEND_FACTOR_ONE_MINUS_DST_COLOR      = 0x0307, // GL_ONE_MINUS_DST_COLOR
  GL_BLEND_FACTOR_SRC_ALPHA                = 0x0302, // GL_SRC_ALPHA
  GL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA      = 0x0303, // GL_ONE_MINUS_SRC_ALPHA
  GL_BLEND_FACTOR_DST_ALPHA                = 0x0304, // GL_DST_ALPHA
  GL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA      = 0x0305, // GL_ONE_MINUS_DST_ALPHA
  GL_BLEND_FACTOR_CONSTANT_COLOR           = 0x8001, // GL_CONSTANT_COLOR
  GL_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR = 0x8002, // GL_ONE_MINUS_CONSTANT_COLOR
  GL_BLEND_FACTOR_CONSTANT_ALPHA           = 0x8003, // GL_CONSTANT_ALPHA
  GL_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA = 0x8004, // GL_ONE_MINUS_CONSTANT_ALPHA
  GL_BLEND_FACTOR_SRC_ALPHA_SATURATE       = 0x0308, // GL_SRC_ALPHA_SATURATE
};

enum GL_BLEND_OP {
  GL_BLEND_OP_ADD              = 0x8006, // GL_FUNC_ADD
  GL_BLEND_OP_SUBTRACT         = 0x800A, // GL_FUNC_SUBTRACT
  GL_BLEND_OP_REVERSE_SUBTRACT = 0x800B, // GL_FUNC_REVERSE_SUBTRACT
  GL_BLEND_OP_MIN              = 0x8007, // GL_MIN
  GL_BLEND_OP_MAX              = 0x8008, // GL_MAX
};

// ---

enum GL_GET_PROGRAM_ERROR {
  GL_PROGRAM_ERROR_POSITION_ARB = 0x864B,
  GL_PROGRAM_ERROR_STRING_ARB   = 0x8874,
};

enum GL_GET_ERROR {
  GL_NO_ERROR          = 0,
  GL_INVALID_ENUM      = 0x0500,
  GL_INVALID_VALUE     = 0x0501,
  GL_INVALID_OPERATION = 0x0502,
  GL_STACK_OVERFLOW    = 0x0503,
  GL_STACK_UNDERFLOW   = 0x0504,
  GL_OUT_OF_MEMORY     = 0x0505,
};

void glGetIntegerv(unsigned pname, int *params);
const unsigned char *glGetString(unsigned name);
enum GL_GET_ERROR glGetError(void);

#else
#include "gl_portable_win.h"
#endif

struct GL_GRAPHICS_PIPELINE_STATE_INFO {
  unsigned             programVertex;
  unsigned             programFragment;
  GlBool32             rasterizationDepthClampEnable;
  unsigned             rasterizationCullMode; /*enum GL_CULL_MODE*/
  unsigned             rasterizationFrontFace; /*enum GL_FRONT_FACE*/
  GlBool32             rasterizationDepthBiasEnable;
  float                rasterizationDepthBiasConstantFactor;
  float                rasterizationDepthBiasSlopeFactor;
  GlBool32             multisampleEnable;
  GlBool32             multisampleAlphaToCoverageEnable;
  GlBool32             multisampleAlphaToOneEnable;
  GlBool32             depthStencilDepthTestEnable;
  GlBool32             depthStencilDepthWriteEnable;
  unsigned             depthStencilDepthCompareOp; /*enum GL_COMPARE_OP*/
  GlBool32             depthStencilStencilTestEnable;
  unsigned             depthStencilFrontAndBackFailOp; /*enum GL_STENCIL_OP*/
  unsigned             depthStencilFrontAndBackPassOp; /*enum GL_STENCIL_OP*/
  unsigned             depthStencilFrontAndBackDepthFailOp; /*enum GL_STENCIL_OP*/
  unsigned             depthStencilFrontAndBackCompareOp; /*enum GL_COMPARE_OP*/
  unsigned             depthStencilFrontAndBackCompareMask;
  unsigned             depthStencilFrontAndBackWriteMask;
  unsigned             depthStencilFrontAndBackReference;
  GlBool32             colorBlendLogicOpEnable;
  unsigned             colorBlendLogicOp; /*enum GL_LOGIC_OP*/
  float                colorBlendBlendConstants[4];
  GlBool32             colorBlendBlendEnable;
  unsigned             colorBlendSrcColorAndAlphaBlendFactor; /*enum GL_BLEND_FACTOR*/
  unsigned             colorBlendDstColorAndAlphaBlendFactor; /*enum GL_BLEND_FACTOR*/
  unsigned             colorBlendColorAndAlphaBlendOp; /*enum GL_BLEND_OP*/
  GlBool32             colorBlendColorWriteEnableR;
  GlBool32             colorBlendColorWriteEnableG;
  GlBool32             colorBlendColorWriteEnableB;
  GlBool32             colorBlendColorWriteEnableA;
};

API void glGenPipelines(int n, unsigned *pipelines);
API void glDeletePipelines(int n, const unsigned *pipelines);
API void glBindPipeline(unsigned pipeline);
API void glCompilePipeline(unsigned pipeline, const struct GL_GRAPHICS_PIPELINE_STATE_INFO *pipeline_info);

#ifdef __cplusplus
}
#endif

#endif

#include "gl_portable.h"

#ifndef _WIN32
void glCullFace(unsigned mode);
void glFrontFace(unsigned mode);
void glPolygonOffset(float factor, float units);
void glLineWidth(float width);
void glDepthMask(unsigned char flag);
void glDepthFunc(unsigned func);
void glStencilOp(unsigned fail, unsigned zfail, unsigned zpass);
void glStencilFunc(unsigned func, int ref, unsigned mask);
void glStencilMask(unsigned mask);
void glLogicOp(unsigned opcode);
void glBlendColor(float red, float green, float blue, float alpha);
void glBlendFunc(unsigned sfactor, unsigned dfactor);
void glBlendEquation(unsigned mode);
void glColorMask(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha);
#endif

void glGenPipelines(int n, unsigned *pipelines) {
  unsigned list_start = glGenLists(n);
  for (int i = 0; i < n; i += 1) {
    pipelines[i] = list_start + i;
  }
}

void glDeletePipelines(int n, const unsigned *pipelines) {
  for (int i = 0; i < n; i += 1) {
    glDeleteLists(pipelines[i], 1);
  }
}

void glBindPipeline(unsigned pipeline) {
  glCallList(pipeline);
}

void glCompilePipeline(unsigned pipeline, const struct GL_GRAPHICS_PIPELINE_STATE_INFO *pipeline_info) {
  glNewList(pipeline, GL_COMPILE);

  glEnable(0x0C11); // GL_SCISSOR_TEST
  pipeline_info->rasterizationDepthClampEnable == 1 ? glEnable(0x864F) : glDisable(0x864F);  // GL_DEPTH_CLAMP
  if (pipeline_info->rasterizationCullMode == GL_CULL_MODE_NONE) {
    glDisable(0x0B44); // GL_CULL_FACE
  } else {
    glEnable(0x0B44);  // GL_CULL_FACE
    glCullFace(pipeline_info->rasterizationCullMode);
  }
  glFrontFace(pipeline_info->rasterizationFrontFace);
  pipeline_info->rasterizationDepthBiasEnable == 1 ? glEnable(0x8037) : glDisable(0x8037); // GL_POLYGON_OFFSET_FILL
  glPolygonOffset(pipeline_info->rasterizationDepthBiasConstantFactor, pipeline_info->rasterizationDepthBiasSlopeFactor);
  glLineWidth(pipeline_info->rasterizationLineWidth);
  pipeline_info->multisampleEnable == 1 ? glEnable(0x809D)  : glDisable(0x809D); // GL_MULTISAMPLE
  pipeline_info->multisampleAlphaToCoverageEnable == 1 ? glEnable(0x809E) : glDisable(0x809E); // GL_SAMPLE_ALPHA_TO_COVERAGE
  pipeline_info->multisampleAlphaToOneEnable == 1 ? glEnable(0x809F) : glDisable(0x809F); // GL_SAMPLE_ALPHA_TO_ONE
  pipeline_info->depthStencilDepthTestEnable == 1 ? glEnable(0x0B71) : glDisable(0x0B71); // GL_DEPTH_TEST
  glDepthMask(pipeline_info->depthStencilDepthWriteEnable == 1 ? 1 : 0);
  glDepthFunc(pipeline_info->depthStencilDepthCompareOp);
  pipeline_info->depthStencilStencilTestEnable == 1 ? glEnable(0x0B90) : glDisable(0x0B90); // GL_STENCIL_TEST
  glStencilOp(pipeline_info->depthStencilFrontAndBackFailOp, pipeline_info->depthStencilFrontAndBackDepthFailOp, pipeline_info->depthStencilFrontAndBackPassOp);
  glStencilFunc(pipeline_info->depthStencilFrontAndBackCompareOp, pipeline_info->depthStencilFrontAndBackReference, pipeline_info->depthStencilFrontAndBackCompareMask);
  glStencilMask(pipeline_info->depthStencilFrontAndBackWriteMask);
  pipeline_info->colorBlendLogicOpEnable == 1 ? glEnable(0x0BF2) : glDisable(0x0BF2); // GL_COLOR_LOGIC_OP
  glLogicOp(pipeline_info->colorBlendLogicOp);
  glBlendColor(pipeline_info->colorBlendBlendConstants[0], pipeline_info->colorBlendBlendConstants[1], pipeline_info->colorBlendBlendConstants[2], pipeline_info->colorBlendBlendConstants[3]);
  pipeline_info->colorBlendBlendEnable == 1 ? glEnable(0x0BE2) : glDisable(0x0BE2); // GL_BLEND
  glBlendFunc(pipeline_info->colorBlendSrcColorAndAlphaBlendFactor, pipeline_info->colorBlendDstColorAndAlphaBlendFactor);
  glBlendEquation(pipeline_info->colorBlendColorAndAlphaBlendOp);
  glColorMask(pipeline_info->colorBlendColorWriteEnableR == 1 ? 1 : 0, pipeline_info->colorBlendColorWriteEnableG == 1 ? 1 : 0, pipeline_info->colorBlendColorWriteEnableB == 1 ? 1 : 0, pipeline_info->colorBlendColorWriteEnableA == 1 ? 1 : 0);
  glBindProgramARB(GL_VERTEX_PROGRAM_ARB,   pipeline_info->programVertex);
  glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, pipeline_info->programFragment);

  glEndList();
}

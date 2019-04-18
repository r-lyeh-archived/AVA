#include <engine.h>

const float mesh_vertices[] = {
#include "00_mesh_vertices.h"
};

int main() {
    window_create(0.75f, WINDOW_SQUARED | WINDOW_MSAA4);

    glEnable(GL_VERTEX_PROGRAM_ARB);
    glEnable(GL_FRAGMENT_PROGRAM_ARB);

    char vs_string[] =
        "!!ARBvp1.0"
        "MOV result.color, vertex.color;"
        "MOV result.position, vertex.position;"
        "END";

    char ps_string[] =
        "!!ARBfp1.0"
        "MOV result.color, fragment.color;"
        "END";

    unsigned vs_program = 0;
    glGenProgramsARB(1, &vs_program);
    glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vs_program);
    glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, sizeof(vs_string)-1, vs_string);
    glBindProgramARB(GL_VERTEX_PROGRAM_ARB, 0);

    unsigned ps_program = 0;
    glGenProgramsARB(1, &ps_program);
    glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, ps_program);
    glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, sizeof(ps_string)-1, ps_string);
    glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, 0);

    struct GL_GRAPHICS_PIPELINE_STATE_INFO pipeline_info = {0};
    pipeline_info.programVertex                         = vs_program;
    pipeline_info.programFragment                       = ps_program;
    pipeline_info.rasterizationDepthClampEnable         = 0;
    pipeline_info.rasterizationCullMode                 = GL_CULL_MODE_NONE;
    pipeline_info.rasterizationFrontFace                = GL_FRONT_FACE_COUNTER_CLOCKWISE;
    pipeline_info.rasterizationDepthBiasEnable          = 0;
    pipeline_info.rasterizationDepthBiasConstantFactor  = 0;
    pipeline_info.rasterizationDepthBiasSlopeFactor     = 0;
    pipeline_info.rasterizationLineWidth                = 1;
    pipeline_info.multisampleEnable                     = 1;
    pipeline_info.multisampleAlphaToCoverageEnable      = 0;
    pipeline_info.multisampleAlphaToOneEnable           = 0;
    pipeline_info.depthStencilDepthTestEnable           = 0;
    pipeline_info.depthStencilDepthWriteEnable          = 0;
    pipeline_info.depthStencilDepthCompareOp            = GL_COMPARE_OP_GREATER_OR_EQUAL;
    pipeline_info.depthStencilStencilTestEnable         = 0;
    pipeline_info.depthStencilFrontAndBackFailOp        = GL_STENCIL_OP_KEEP;
    pipeline_info.depthStencilFrontAndBackPassOp        = GL_STENCIL_OP_KEEP;
    pipeline_info.depthStencilFrontAndBackDepthFailOp   = GL_STENCIL_OP_KEEP;
    pipeline_info.depthStencilFrontAndBackCompareOp     = GL_COMPARE_OP_NEVER;
    pipeline_info.depthStencilFrontAndBackCompareMask   = 0;
    pipeline_info.depthStencilFrontAndBackWriteMask     = 0;
    pipeline_info.depthStencilFrontAndBackReference     = 0;
    pipeline_info.colorBlendLogicOpEnable               = 0;
    pipeline_info.colorBlendLogicOp                     = GL_LOGIC_OP_CLEAR;
    pipeline_info.colorBlendBlendConstants[0]           = 0;
    pipeline_info.colorBlendBlendConstants[1]           = 0;
    pipeline_info.colorBlendBlendConstants[2]           = 0;
    pipeline_info.colorBlendBlendConstants[3]           = 0;
    pipeline_info.colorBlendBlendEnable                 = 0;
    pipeline_info.colorBlendSrcColorAndAlphaBlendFactor = GL_BLEND_FACTOR_ZERO;
    pipeline_info.colorBlendDstColorAndAlphaBlendFactor = GL_BLEND_FACTOR_ZERO;
    pipeline_info.colorBlendColorAndAlphaBlendOp        = GL_BLEND_OP_ADD;
    pipeline_info.colorBlendColorWriteEnableR           = 1;
    pipeline_info.colorBlendColorWriteEnableG           = 1;
    pipeline_info.colorBlendColorWriteEnableB           = 1;
    pipeline_info.colorBlendColorWriteEnableA           = 1;
    unsigned pipeline = 0;
    glGenPipelines(1, &pipeline);
    glCompilePipeline(pipeline, &pipeline_info);

    unsigned list = glGenLists(1);
    glNewList(list, GL_COMPILE);
        glClearDepthf(1);
        glClearStencil(0);
        glClearColor(0, 0, 0.05, 1);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glBindPipeline(pipeline);
        glBegin(GL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        for (int i = 0, mesh_vertices_count = countof(mesh_vertices) / 3; i < mesh_vertices_count; i += 1) {
            float scale_x = 0.5;
            float scale_y = 0.5;
            float scale_z = 0.5;
            glColor4f(i * 0.00025, 0, 0.1, 1);
            glVertex4f(mesh_vertices[i * 3 + 0] * scale_x,
                                 mesh_vertices[i * 3 + 1] * scale_y,
                                 mesh_vertices[i * 3 + 2] * scale_z, 1);
        }
        glEnd();
    glEndList();

    while( window_update() ) {
        int *rect = window_size();
        glViewport(0, 0, rect[0], rect[1]);
        glScissor( 0, 0, rect[0], rect[1]);
        glCallList(list);
        window_swap(NULL);
    }

    window_destroy();
}

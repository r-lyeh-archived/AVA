#if 0
rm -f a.out
clang -nostdlib -fno-stack-protector ../stdlib/syscall.s ../stdlib/main.s main.c -lglfw -lGL
./a.out
exit
#endif

#include <engine.h>

int main() {
    window_create(0.75f, WINDOW_SQUARED);

    glEnable(GL_VERTEX_PROGRAM_ARB);
    glEnable(GL_FRAGMENT_PROGRAM_ARB);

    typedef struct {
        unsigned char r, g, b, a;
    } color;

    const color colors[] = {
        [0] = {  0,   0,   0, 255},
        [1] = {255,   0,   0, 255},
        [2] = {  0, 255,   0, 255},
        [3] = {  0,   0, 255, 255},
        [4] = {255,   0, 255, 255},
    };

#define _ 0

    const int image1[] = {
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,2,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,3,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,4,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
    };

    const int image2[] = {
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,1,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,2,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,3,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,4,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
        _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,
    };

    color texture1[countof(image1)];
    for (int i = 0; i < countof(image1); i += 1) {
        texture1[i] = colors[image1[i]];
    }

    color texture2[countof(image2)];
    for (int i = 0; i < countof(image2); i += 1) {
        texture2[i] = colors[image2[i]];
    }

    char vs_string[] =
        "!!ARBvp1.0"
        "TEMP R0;"
        "MAD R0.xy, vertex.position, 0.5, 0.5;"
        "MOV result.texcoord[0].x, R0;"
        "ADD result.texcoord[0].y,-R0, 1;"
        "MOV result.position, vertex.position;"
        "END";

    char ps_string[] =
        "!!ARBfp1.0"
        "TEX result.color, fragment.texcoord[0], texture[0], 2D;"
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
    pipeline_info.multisampleEnable                     = 0;
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

    unsigned texture1_id = 0;
    glGenTextures(1, &texture1_id);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D,                              texture1_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,     GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,     GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,         GL_TEXTURE_ADDRESS_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,         GL_TEXTURE_ADDRESS_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP,        0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexImage2D(GL_TEXTURE_2D, 0,
                             GL_RGBA,
                             (int)sqrtf(countof(image1)),    // Assuming square texture.,
                             (int)sqrtf(countof(image1)), 0, // Assuming square texture.
                             GL_RGBA,
                             GL_UNSIGNED_BYTE,
                             texture1);
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned texture2_id = 0;
    glGenTextures(1, &texture2_id);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D,                              texture2_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,     GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,     GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,         GL_TEXTURE_ADDRESS_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,         GL_TEXTURE_ADDRESS_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP,        0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexImage2D(GL_TEXTURE_2D, 0,
                             GL_RGBA,
                             (int)sqrtf(countof(image2)),    // Assuming square texture.,
                             (int)sqrtf(countof(image2)), 0, // Assuming square texture.
                             GL_RGBA,
                             GL_UNSIGNED_BYTE,
                             texture2);
    glBindTexture(GL_TEXTURE_2D, 0);

    unsigned list_clear = glGenLists(1);
    glNewList(list_clear, GL_COMPILE);
        glClearDepthf(1);
        glClearStencil(0);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
    glEndList();

    unsigned list1 = glGenLists(1);
    glNewList(list1, GL_COMPILE);
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, texture1_id);
        glBindPipeline(pipeline);
        glBegin(GL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        glVertex4f(-1, 3, 0, 1);
        glVertex4f(-1,-1, 0, 1);
        glVertex4f( 3,-1, 0, 1);
        glEnd();
    glEndList();

    unsigned list2 = glGenLists(1);
    glNewList(list2, GL_COMPILE);
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, texture2_id);
        glBindPipeline(pipeline);
        glBegin(GL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        glVertex4f(-1, 3, 0, 1);
        glVertex4f(-1,-1, 0, 1);
        glVertex4f( 3,-1, 0, 1);
        glEnd();
    glEndList();

    for (int frame = 0; window_update(); ) {
        int *rect = window_size();

        glViewport(0, 0, rect[0], rect[1]);
        glScissor( 0, 0, rect[0], rect[1]);
        glCallList(list_clear);

        glViewport(0, 0, rect[0], rect[1]);
        glScissor( 0, 0, rect[0], rect[1]);
        frame % 2 ? glCallList(list1) : glCallList(list2);

        window_swap(NULL);
        frame += 1;
    }

    window_destroy();
}

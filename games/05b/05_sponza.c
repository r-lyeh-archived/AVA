#if 0
rm -f a.out
if [ ! -e meshes.o ]; then ./meshes.c; fi
clang -nostdlib -fno-stack-protector ../stdlib/syscall.s ../stdlib/main.s meshes.o main.c -lglfw -lGL
./a.out
exit
#endif

#include <engine.h>

enum mesh_names {
#include "data/sponza/mesh_enums.h"
SUBMESH_COUNT
};

const char *mesh_texture_names[] = {
#include "data/sponza/mesh_texture_names.h"
};

const int mesh_texture_ids[] = {
#include "data/sponza/mesh_texture_ids.h"
};

const float mesh_vertices[] = {
#include "data/sponza/mesh_vertices.h"
};

const float mesh_uvs[] = {
#include "data/sponza/mesh_uvs.h"
};

const int submesh_vertex_begin[] = {
#include "data/sponza/submesh_vertex_begin.h"
};

const int submesh_vertex_end[] = {
#include "data/sponza/submesh_vertex_end.h"
};

extern const int   mesh_texture_ids[];
extern const float mesh_vertices[];
extern const float mesh_uvs[];
extern const int   submesh_vertex_begin[];
extern const int   submesh_vertex_end[];

#define countof(x) ( sizeof(x) / sizeof((x)[0]) )

int main() {
    window_create(0.75f, WINDOW_NO_MOUSE);

    glEnable(GL_VERTEX_PROGRAM_ARB);
    glEnable(GL_FRAGMENT_PROGRAM_ARB);

    char vs_string[] =
        "!!ARBvp1.0"
        "TEMP pos;"
        "TEMP R0;"
        "ADD pos, vertex.position, -program.local[0];"
        "MUL R0.xyzw, pos.xzzx, program.local[1].xyxy;"
        "ADD pos.x, R0.x,-R0.y;"
        "ADD pos.z, R0.z, R0.w;"
        "MUL R0.xyzw, pos.yzzy, program.local[1].zwzw;"
        "ADD pos.y, R0.x,-R0.y;"
        "ADD pos.z, R0.z, R0.w;"
        "MUL pos.xy, pos, {0.613861, 1.091308, 1, 1};"
        "MOV result.texcoord[0], vertex.texcoord[0];"
        "MOV result.position.xyw, pos.xyzz;"
        "MOV result.position.z, 0.1;"
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
    pipeline_info.rasterizationCullMode                 = GL_CULL_MODE_BACK;
    pipeline_info.rasterizationFrontFace                = GL_FRONT_FACE_COUNTER_CLOCKWISE;
    pipeline_info.rasterizationDepthBiasEnable          = 0;
    pipeline_info.rasterizationDepthBiasConstantFactor  = 0;
    pipeline_info.rasterizationDepthBiasSlopeFactor     = 0;
    pipeline_info.multisampleEnable                     = 1;
    pipeline_info.multisampleAlphaToCoverageEnable      = 0;
    pipeline_info.multisampleAlphaToOneEnable           = 0;
    pipeline_info.depthStencilDepthTestEnable           = 1;
    pipeline_info.depthStencilDepthWriteEnable          = 1;
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

    struct GL_GRAPHICS_PIPELINE_STATE_INFO pipeline_with_blend_info = {0};
    pipeline_with_blend_info.programVertex                         = vs_program;
    pipeline_with_blend_info.programFragment                       = ps_program;
    pipeline_with_blend_info.rasterizationDepthClampEnable         = 0;
    pipeline_with_blend_info.rasterizationCullMode                 = GL_CULL_MODE_NONE;
    pipeline_with_blend_info.rasterizationFrontFace                = GL_FRONT_FACE_COUNTER_CLOCKWISE;
    pipeline_with_blend_info.rasterizationDepthBiasEnable          = 0;
    pipeline_with_blend_info.rasterizationDepthBiasConstantFactor  = 0;
    pipeline_with_blend_info.rasterizationDepthBiasSlopeFactor     = 0;
    pipeline_with_blend_info.multisampleEnable                     = 1;
    pipeline_with_blend_info.multisampleAlphaToCoverageEnable      = 1;
    pipeline_with_blend_info.multisampleAlphaToOneEnable           = 0;
    pipeline_with_blend_info.depthStencilDepthTestEnable           = 1;
    pipeline_with_blend_info.depthStencilDepthWriteEnable          = 1;
    pipeline_with_blend_info.depthStencilDepthCompareOp            = GL_COMPARE_OP_GREATER_OR_EQUAL;
    pipeline_with_blend_info.depthStencilStencilTestEnable         = 0;
    pipeline_with_blend_info.depthStencilFrontAndBackFailOp        = GL_STENCIL_OP_KEEP;
    pipeline_with_blend_info.depthStencilFrontAndBackPassOp        = GL_STENCIL_OP_KEEP;
    pipeline_with_blend_info.depthStencilFrontAndBackDepthFailOp   = GL_STENCIL_OP_KEEP;
    pipeline_with_blend_info.depthStencilFrontAndBackCompareOp     = GL_COMPARE_OP_NEVER;
    pipeline_with_blend_info.depthStencilFrontAndBackCompareMask   = 0;
    pipeline_with_blend_info.depthStencilFrontAndBackWriteMask     = 0;
    pipeline_with_blend_info.depthStencilFrontAndBackReference     = 0;
    pipeline_with_blend_info.colorBlendLogicOpEnable               = 0;
    pipeline_with_blend_info.colorBlendLogicOp                     = GL_LOGIC_OP_CLEAR;
    pipeline_with_blend_info.colorBlendBlendConstants[0]           = 0;
    pipeline_with_blend_info.colorBlendBlendConstants[1]           = 0;
    pipeline_with_blend_info.colorBlendBlendConstants[2]           = 0;
    pipeline_with_blend_info.colorBlendBlendConstants[3]           = 0;
    pipeline_with_blend_info.colorBlendBlendEnable                 = 1;
    pipeline_with_blend_info.colorBlendSrcColorAndAlphaBlendFactor = GL_BLEND_FACTOR_SRC_ALPHA;
    pipeline_with_blend_info.colorBlendDstColorAndAlphaBlendFactor = GL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    pipeline_with_blend_info.colorBlendColorAndAlphaBlendOp        = GL_BLEND_OP_ADD;
    pipeline_with_blend_info.colorBlendColorWriteEnableR           = 1;
    pipeline_with_blend_info.colorBlendColorWriteEnableG           = 1;
    pipeline_with_blend_info.colorBlendColorWriteEnableB           = 1;
    pipeline_with_blend_info.colorBlendColorWriteEnableA           = 1;
    unsigned pipeline_with_blend = 0;
    glGenPipelines(1, &pipeline_with_blend);
    glCompilePipeline(pipeline_with_blend, &pipeline_with_blend_info);

    const int mesh_texture_names_count = countof(mesh_texture_names);

    ALLOCA(image, textures_mem, mesh_texture_names_count);

    for (int i = 1; i < mesh_texture_names_count; i += 1) {
        image_loadfile( &textures_mem[i], mesh_texture_names[i], IMAGE_RGBA | IMAGE_U8);
    }

    ALLOCA(void *, textures, SUBMESH_COUNT);
    ALLOCA(int , textures_w, SUBMESH_COUNT);
    ALLOCA(int , textures_h, SUBMESH_COUNT);

    for (int i = 0; i < SUBMESH_COUNT; i += 1) {
        int id = mesh_texture_ids[i];
        textures[i]   = textures_mem[id].pixels;
        textures_w[i] = textures_mem[id].w;
        textures_h[i] = textures_mem[id].h;
    }

    unsigned submesh_texures[SUBMESH_COUNT] = {0};
    glGenTextures(SUBMESH_COUNT, submesh_texures);

    unsigned char nil_texture[4 * 4] = {146, 146, 146, 255,
                                                                            146, 146, 146, 255,
                                                                            146, 146, 146, 255,
                                                                            146, 146, 146, 255};

    for (int i = 0; i < SUBMESH_COUNT; i += 1) {
        if (textures[i] == NULL) {
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, submesh_texures[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,     GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,     GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,         GL_TEXTURE_ADDRESS_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,         GL_TEXTURE_ADDRESS_CLAMP);
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP,        0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 1);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, nil_texture);
            glBindTexture(GL_TEXTURE_2D, 0);
            continue;
        }
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, submesh_texures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_TEXTURE_ADDRESS_WRAP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_TEXTURE_ADDRESS_WRAP);
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP,    1);
        if (// Foliage:
                i == SPONZA_370_001 || i == SPONZA_369_001 || i == SPONZA_368_001 || i == SPONZA_367_001 || i == SPONZA_366_001 ||
                i == SPONZA_01_001  || i == SPONZA_372_001 || i == SPONZA_371_001 || i == SPONZA_276     || i == SPONZA_275     ||
                i == SPONZA_281     || i == SPONZA_00      || i == SPONZA_280     || i == SPONZA_279     || i == SPONZA_278     ||
                i == SPONZA_277     ||
                // Chains:
                i == SPONZA_341     || i == SPONZA_339     || i == SPONZA_340     || i == SPONZA_342     || i == SPONZA_333     ||
                i == SPONZA_331     || i == SPONZA_332     || i == SPONZA_330     || i == SPONZA_360     || i == SPONZA_358     ||
                i == SPONZA_357     || i == SPONZA_359     || i == SPONZA_348     || i == SPONZA_350     || i == SPONZA_351     ||
                i == SPONZA_349)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 1);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 8);
        }
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textures_w[i], textures_h[i], 0, GL_RGBA, GL_UNSIGNED_BYTE, textures[i]);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    for (int i = 1; i < mesh_texture_names_count; i += 1) {
        image_destroy(&textures_mem[i]);
    }

    for (int i = 0; i < SUBMESH_COUNT; i += 1) {
        textures[i]   = NULL;
        textures_w[i] = 0;
        textures_h[i] = 0;
    }

    unsigned list_clear = glGenLists(1);
    glNewList(list_clear, GL_COMPILE);
        glClearDepthf(0);
        glClearStencil(0);
        glClearColor(0.98, 0.98, 0.98, 1);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glEndList();

    unsigned list = glGenLists(1);
    glNewList(list, GL_COMPILE);
        glBindPipeline(pipeline);
        for (int j = 0; j < SUBMESH_COUNT; j += 1) {
            if (// Foliage:
                    j == SPONZA_370_001 || j == SPONZA_369_001 || j == SPONZA_368_001 || j == SPONZA_367_001 || j == SPONZA_366_001 ||
                    j == SPONZA_01_001  || j == SPONZA_372_001 || j == SPONZA_371_001 || j == SPONZA_276     || j == SPONZA_275     ||
                    j == SPONZA_281     || j == SPONZA_00      || j == SPONZA_280     || j == SPONZA_279     || j == SPONZA_278     ||
                    j == SPONZA_277     ||
                    // Chains:
                    j == SPONZA_341     || j == SPONZA_339     || j == SPONZA_340     || j == SPONZA_342     || j == SPONZA_333     ||
                    j == SPONZA_331     || j == SPONZA_332     || j == SPONZA_330     || j == SPONZA_360     || j == SPONZA_358     ||
                    j == SPONZA_357     || j == SPONZA_359     || j == SPONZA_348     || j == SPONZA_350     || j == SPONZA_351     ||
                    j == SPONZA_349)
            {
                continue;
            } else {
            }
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, submesh_texures[j]);
            glBegin(GL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
            for (int i = submesh_vertex_begin[j]; i < submesh_vertex_end[j]; i += 1) {
                glMultiTexCoord4f(GL_TEXTURE0 + 0, mesh_uvs[i * 2 + 0],
                                                                         1.0 - mesh_uvs[i * 2 + 1], 0, 0);
                glVertex4f(mesh_vertices[i * 3 + 0],
                                     mesh_vertices[i * 3 + 1],
                                     mesh_vertices[i * 3 + 2], 1);
            }
            glEnd();
        }
    glEndList();

    unsigned list_with_blend = glGenLists(1);
    glNewList(list_with_blend, GL_COMPILE);
        glBindPipeline(pipeline_with_blend);
        for (int j = 0; j < SUBMESH_COUNT; j += 1) {
            if (// Foliage:
                    j == SPONZA_370_001 || j == SPONZA_369_001 || j == SPONZA_368_001 || j == SPONZA_367_001 || j == SPONZA_366_001 ||
                    j == SPONZA_01_001  || j == SPONZA_372_001 || j == SPONZA_371_001 || j == SPONZA_276     || j == SPONZA_275     ||
                    j == SPONZA_281     || j == SPONZA_00      || j == SPONZA_280     || j == SPONZA_279     || j == SPONZA_278     ||
                    j == SPONZA_277     ||
                    // Chains:
                    j == SPONZA_341     || j == SPONZA_339     || j == SPONZA_340     || j == SPONZA_342     || j == SPONZA_333     ||
                    j == SPONZA_331     || j == SPONZA_332     || j == SPONZA_330     || j == SPONZA_360     || j == SPONZA_358     ||
                    j == SPONZA_357     || j == SPONZA_359     || j == SPONZA_348     || j == SPONZA_350     || j == SPONZA_351     ||
                    j == SPONZA_349)
            {
            } else {
                continue;
            }
            glActiveTexture(GL_TEXTURE0 + 0);
            glBindTexture(GL_TEXTURE_2D, submesh_texures[j]);
            glBegin(GL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
            for (int i = submesh_vertex_begin[j]; i < submesh_vertex_end[j]; i += 1) {
                glMultiTexCoord4f(GL_TEXTURE0 + 0, mesh_uvs[i * 2 + 0],
                                                                         1.0 - mesh_uvs[i * 2 + 1], 0, 0);
                glVertex4f(mesh_vertices[i * 3 + 0],
                                     mesh_vertices[i * 3 + 1],
                                     mesh_vertices[i * 3 + 2], 1);
            }
            glEnd();
        }
    glEndList();

    for (float pos_x = 0, pos_y = 7, pos_z = 0, rot_x = 0, rot_y = 0; window_update(); ) {
        int *rect = window_size();

        double mouse_x = 0;
        double mouse_y = 0;
        static double mouse_x_prev = 0;
        static double mouse_y_prev = 0;
        mouse_x = mouse('X');
        mouse_y = mouse('Y');

        float mouse_disp_x = (float)(mouse_x_prev - mouse_x) * 0.0035f;
        float mouse_disp_y = (float)(mouse_y - mouse_y_prev) * 0.0035f;

        rot_x += mouse_disp_x;
        rot_y += mouse_disp_y;

        float move_vec_x = 0;
        float move_vec_y = 0;
        float move_vec_z = 0;

        move_vec_x += key('D');
        move_vec_x -= key('A');

        move_vec_y += key('E');
        move_vec_y -= key('Q');

        move_vec_z += key('W');
        move_vec_z -= key('S');

        float move_vec_len = sqrtf(move_vec_x*move_vec_x + move_vec_y*move_vec_y + move_vec_z*move_vec_z);
        if (move_vec_len != 0) {
            move_vec_x /= move_vec_len;
            move_vec_y /= move_vec_len;
            move_vec_z /= move_vec_len;
        }

        {
            float rot_cos = cosf(rot_y);
            float rot_sin = sinf(rot_y);
            float pos_y = move_vec_y;
            float pos_z = move_vec_z;
            move_vec_y = pos_y * rot_cos - pos_z * rot_sin;
            move_vec_z = pos_z * rot_cos + pos_y * rot_sin;
        }

        {
            float rot_cos = cosf(rot_x);
            float rot_sin = sinf(rot_x);
            float pos_x = move_vec_x;
            float pos_z = move_vec_z;
            move_vec_x = pos_x * rot_cos - pos_z * rot_sin;
            move_vec_z = pos_z * rot_cos + pos_x * rot_sin;
        }

        pos_x += move_vec_x * 0.225;
        pos_y += move_vec_y * 0.225;
        pos_z += move_vec_z * 0.225;

        glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vs_program);
        glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 0, pos_x, pos_y, pos_z, 0);
        glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 1, cosf(-rot_x), sinf(-rot_x), cosf(-rot_y), sinf(-rot_y));

        glViewport(0, 0, rect[0], rect[1]);
        glScissor( 0, 0, rect[0], rect[1]);
        glCallList(list_clear);

        glViewport(0, 0, rect[0], rect[1]);
        glScissor( 0, 0, rect[0], rect[1]);
        glCallList(list);

        glViewport(0, 0, rect[0], rect[1]);
        glScissor( 0, 0, rect[0], rect[1]);
        glCallList(list_with_blend);

        static void *pixels = 0;
        window_swap(&pixels);
        network_sendbuf( pixels, rect[0], rect[1], 3, 332 ); // 888, 332, 242, 7755

        mouse_x_prev = mouse_x;
        mouse_y_prev = mouse_y;
    }

    window_destroy();
}

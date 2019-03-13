#if 0
rm -f a.out
clang -nostdlib -fno-stack-protector ../stdlib/syscall.s ../stdlib/main.s main.c -lglfw -lGL
./a.out
exit
#endif


#include <engine.h>

const float mesh_vertices[] = {
#include "07_mesh_vertices.h"
};

const float mesh_normals[] = {
#include "07_mesh_normals.h"
};

#define countof(x) ( sizeof(x) / sizeof((x)[0]) )


int main() {

    window_create( 75.0f, WINDOW_SQUARED | WINDOW_NO_MOUSE );

    glEnable(GL_VERTEX_PROGRAM_ARB);
    glEnable(GL_FRAGMENT_PROGRAM_ARB);

    char vs_skybox_string[] =
        "!!ARBvp1.0"
        "TEMP pos;"
        "TEMP R0;"
        "MOV pos, vertex.position;"
        "MUL R0.xyzw, pos.xzzx, program.local[0].xyxy;"
        "ADD pos.x, R0.x,-R0.y;"
        "ADD pos.z, R0.z, R0.w;"
        "MUL R0.xyzw, pos.yzzy, program.local[0].zwzw;"
        "ADD pos.y, R0.x,-R0.y;"
        "ADD pos.z, R0.z, R0.w;"
        "MOV result.texcoord[0], vertex.position;"
        "MOV result.position.xyw, pos.xyzz;"
        "MOV result.position.z, 0.1;"
        "END";

    char ps_skybox_string[] =
        "!!ARBfp1.0"
        "TEX result.color, fragment.texcoord[0], texture[0], CUBE;"
        "END";

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
        "MOV result.texcoord[0], vertex.position;"
        "MOV result.texcoord[1], vertex.normal;"
        "MOV result.position.xyw, pos.xyzz;"
        "MOV result.position.z, 0.1;"
        "END";

    char ps_string[] =
        "!!ARBfp1.0"
        "TEMP R0;"
        "TEMP R1;"
        "ADD R0.xyz, fragment.texcoord[0], -program.local[0];"
        "DP3 R0.w, fragment.texcoord[1], R0;"
        "MUL R1.xyz, fragment.texcoord[1], R0.w;"
        "MAD R0.xyz, -R1, program.local[0].y, R0;"
        "TEX result.color, R0, texture[0], CUBE;"
        "END";

    unsigned vs_skybox_program = 0;
    glGenProgramsARB(1, &vs_skybox_program);
    glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vs_skybox_program);
    glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, sizeof(vs_skybox_string)-1, vs_skybox_string);
    glBindProgramARB(GL_VERTEX_PROGRAM_ARB, 0);

    unsigned ps_skybox_program = 0;
    glGenProgramsARB(1, &ps_skybox_program);
    glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, ps_skybox_program);
    glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, sizeof(ps_skybox_string)-1, ps_skybox_string);
    glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, 0);

    struct GL_GRAPHICS_PIPELINE_STATE_INFO pipeline_skybox_info = {0};
    pipeline_skybox_info.programVertex                         = vs_skybox_program;
    pipeline_skybox_info.programFragment                       = ps_skybox_program;
    pipeline_skybox_info.rasterizationDepthClampEnable         = 0;
    pipeline_skybox_info.rasterizationCullMode                 = GL_CULL_MODE_NONE;
    pipeline_skybox_info.rasterizationFrontFace                = GL_FRONT_FACE_COUNTER_CLOCKWISE;
    pipeline_skybox_info.rasterizationDepthBiasEnable          = 0;
    pipeline_skybox_info.rasterizationDepthBiasConstantFactor  = 0;
    pipeline_skybox_info.rasterizationDepthBiasSlopeFactor     = 0;
    pipeline_skybox_info.multisampleEnable                     = 0;
    pipeline_skybox_info.multisampleAlphaToCoverageEnable      = 0;
    pipeline_skybox_info.multisampleAlphaToOneEnable           = 0;
    pipeline_skybox_info.depthStencilDepthTestEnable           = 0;
    pipeline_skybox_info.depthStencilDepthWriteEnable          = 0;
    pipeline_skybox_info.depthStencilDepthCompareOp            = GL_COMPARE_OP_GREATER_OR_EQUAL;
    pipeline_skybox_info.depthStencilStencilTestEnable         = 0;
    pipeline_skybox_info.depthStencilFrontAndBackFailOp        = GL_STENCIL_OP_KEEP;
    pipeline_skybox_info.depthStencilFrontAndBackPassOp        = GL_STENCIL_OP_KEEP;
    pipeline_skybox_info.depthStencilFrontAndBackDepthFailOp   = GL_STENCIL_OP_KEEP;
    pipeline_skybox_info.depthStencilFrontAndBackCompareOp     = GL_COMPARE_OP_NEVER;
    pipeline_skybox_info.depthStencilFrontAndBackCompareMask   = 0;
    pipeline_skybox_info.depthStencilFrontAndBackWriteMask     = 0;
    pipeline_skybox_info.depthStencilFrontAndBackReference     = 0;
    pipeline_skybox_info.colorBlendLogicOpEnable               = 0;
    pipeline_skybox_info.colorBlendLogicOp                     = GL_LOGIC_OP_CLEAR;
    pipeline_skybox_info.colorBlendBlendConstants[0]           = 0;
    pipeline_skybox_info.colorBlendBlendConstants[1]           = 0;
    pipeline_skybox_info.colorBlendBlendConstants[2]           = 0;
    pipeline_skybox_info.colorBlendBlendConstants[3]           = 0;
    pipeline_skybox_info.colorBlendBlendEnable                 = 0;
    pipeline_skybox_info.colorBlendSrcColorAndAlphaBlendFactor = GL_BLEND_FACTOR_ZERO;
    pipeline_skybox_info.colorBlendDstColorAndAlphaBlendFactor = GL_BLEND_FACTOR_ZERO;
    pipeline_skybox_info.colorBlendColorAndAlphaBlendOp        = GL_BLEND_OP_ADD;
    pipeline_skybox_info.colorBlendColorWriteEnableR           = 1;
    pipeline_skybox_info.colorBlendColorWriteEnableG           = 1;
    pipeline_skybox_info.colorBlendColorWriteEnableB           = 1;
    pipeline_skybox_info.colorBlendColorWriteEnableA           = 1;
    unsigned pipeline_skybox = 0;
    glGenPipelines(1, &pipeline_skybox);
    glCompilePipeline(pipeline_skybox, &pipeline_skybox_info);

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

    image cubemap_px = {0}; image_loadfile( &cubemap_px, vfs_read("data/cubemap/cubemap_px_right.png"),  IMAGE_RGBA | IMAGE_U8); 
    image cubemap_nx = {0}; image_loadfile( &cubemap_nx, vfs_read("data/cubemap/cubemap_nx_left.png"),   IMAGE_RGBA | IMAGE_U8); 
    image cubemap_py = {0}; image_loadfile( &cubemap_py, vfs_read("data/cubemap/cubemap_py_top.png"),    IMAGE_RGBA | IMAGE_U8); 
    image cubemap_ny = {0}; image_loadfile( &cubemap_ny, vfs_read("data/cubemap/cubemap_ny_bottom.png"), IMAGE_RGBA | IMAGE_U8); 
    image cubemap_pz = {0}; image_loadfile( &cubemap_pz, vfs_read("data/cubemap/cubemap_pz_front.png"),  IMAGE_RGBA | IMAGE_U8); 
    image cubemap_nz = {0}; image_loadfile( &cubemap_nz, vfs_read("data/cubemap/cubemap_nz_back.png"),   IMAGE_RGBA | IMAGE_U8); 
    int cubemap_w = cubemap_px.w, cubemap_h = cubemap_px.h;

    unsigned cubemap = 0;
    glGenTextures(1, &cubemap);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP,                              cubemap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER,     GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,     GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,         GL_TEXTURE_ADDRESS_CLAMP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,         GL_TEXTURE_ADDRESS_CLAMP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP,        1);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0,
                             GL_RGBA,
                             cubemap_w,
                             cubemap_h, 0,
                             GL_RGBA,
                             GL_UNSIGNED_BYTE,
                             cubemap_px.pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0,
                             GL_RGBA,
                             cubemap_w,
                             cubemap_h, 0,
                             GL_RGBA,
                             GL_UNSIGNED_BYTE,
                             cubemap_nx.pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0,
                             GL_RGBA,
                             cubemap_w,
                             cubemap_h, 0,
                             GL_RGBA,
                             GL_UNSIGNED_BYTE,
                             cubemap_py.pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0,
                             GL_RGBA,
                             cubemap_w,
                             cubemap_h, 0,
                             GL_RGBA,
                             GL_UNSIGNED_BYTE,
                             cubemap_ny.pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0,
                             GL_RGBA,
                             cubemap_w,
                             cubemap_h, 0,
                             GL_RGBA,
                             GL_UNSIGNED_BYTE,
                             cubemap_pz.pixels);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0,
                             GL_RGBA,
                             cubemap_w,
                             cubemap_h, 0,
                             GL_RGBA,
                             GL_UNSIGNED_BYTE,
                             cubemap_nz.pixels);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    unsigned list = glGenLists(1);
    glNewList(list, GL_COMPILE);
        glClearDepthf(0);
        glClearStencil(0);
        glClearColor(0.15, 0.15, 0.35, 1);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
        glBindPipeline(pipeline_skybox);
        glBegin(GL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        glVertex4f(-1,-1, 1, 1); glVertex4f(-1,-1,-1, 1); glVertex4f( 1,-1,-1, 1);
        glVertex4f( 1,-1,-1, 1); glVertex4f( 1,-1, 1, 1); glVertex4f(-1,-1, 1, 1);
        glVertex4f(-1, 1,-1, 1); glVertex4f(-1,-1,-1, 1); glVertex4f(-1,-1, 1, 1);
        glVertex4f(-1,-1, 1, 1); glVertex4f(-1, 1, 1, 1); glVertex4f(-1, 1,-1, 1);
        glVertex4f( 1,-1,-1, 1); glVertex4f( 1, 1,-1, 1); glVertex4f( 1, 1, 1, 1);
        glVertex4f( 1, 1, 1, 1); glVertex4f( 1,-1, 1, 1); glVertex4f( 1,-1,-1, 1);
        glVertex4f(-1, 1,-1, 1); glVertex4f(-1, 1, 1, 1); glVertex4f( 1, 1, 1, 1);
        glVertex4f( 1, 1, 1, 1); glVertex4f( 1, 1,-1, 1); glVertex4f(-1, 1,-1, 1);
        glVertex4f(-1,-1, 1, 1); glVertex4f( 1,-1, 1, 1); glVertex4f( 1, 1, 1, 1);
        glVertex4f( 1, 1, 1, 1); glVertex4f(-1, 1, 1, 1); glVertex4f(-1,-1, 1, 1);
        glVertex4f(-1,-1,-1, 1); glVertex4f(-1, 1,-1, 1); glVertex4f( 1,-1,-1, 1);
        glVertex4f( 1,-1,-1, 1); glVertex4f(-1, 1,-1, 1); glVertex4f( 1, 1,-1, 1);
        glEnd();
        glBindPipeline(pipeline);
        glBegin(GL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        for (int i = 0, mesh_vertex_count = countof(mesh_vertices) / 3; i < mesh_vertex_count; i += 1) {
            glNormal3f(mesh_normals[i * 3 + 0],
                                 mesh_normals[i * 3 + 1],
                                 mesh_normals[i * 3 + 2]);
            glVertex4f(mesh_vertices[i * 3 + 0],
                                 mesh_vertices[i * 3 + 1],
                                 mesh_vertices[i * 3 + 2], 1);
        }
        glEnd();
    glEndList();

    for (float pos_x = 0, pos_y = 0, pos_z = -2, rot_x = 0, rot_y = 0; window_update(); ) {
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

        pos_x += move_vec_x * 0.025;
        pos_y += move_vec_y * 0.025;
        pos_z += move_vec_z * 0.025;

        glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vs_skybox_program);
        glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 0, cosf(-rot_x), sinf(-rot_x), cosf(-rot_y), sinf(-rot_y));
        glBindProgramARB(GL_VERTEX_PROGRAM_ARB, vs_program);
        glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 0, pos_x, pos_y, pos_z, 0);
        glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, 1, cosf(-rot_x), sinf(-rot_x), cosf(-rot_y), sinf(-rot_y));
        glViewport(0, 0, rect[0], rect[1]);
        glScissor( 0, 0, rect[0], rect[1]);
        glCallList(list);

        // window_swap( NULL );
        static void *pixels = 0;
        window_swap( &pixels );
        network_sendbuf( pixels, rect[0], rect[1], 3, 332 );

        mouse_x_prev = mouse_x;
        mouse_y_prev = mouse_y;
    }

    window_destroy();
}

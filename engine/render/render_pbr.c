// Original code by @sgorsten (unlicense)
// - rlyeh, public domain.

#ifndef PBR_H
#define PBR_H

typedef struct pbr_tools {
    unsigned spheremap_skybox_prog;
    unsigned cubemap_skybox_prog;
    unsigned irradiance_prog;
    unsigned reflectance_prog;
    unsigned brdf_integration_prog;
    mesh skybox;
} pbr_tools;

API pbr_tools make_pbr();
API GLuint pbr_convert_spheremap2cubemap(const pbr_tools *pbr, GLenum internal_format, GLsizei width, GLuint spheremap);
API GLuint pbr_compute_irradiancemap(const pbr_tools *pbr, GLuint cubemap);
API GLuint pbr_compute_reflectancemap(const pbr_tools *pbr, GLuint cubemap);
API GLuint pbr_compute_brdfmap(pbr_tools *pbr);
API void   pbr_draw_skybox(const pbr_tools *pbr, GLuint cubemap, mat44 skybox_view_proj_matrix);


typedef struct environment {
    GLuint environment_cubemap, irradiance_cubemap, reflectance_cubemap;
} environment;

API environment load_enviroment(const pbr_tools *tools, const char *filename);

API extern const char *preamble;
API extern const char *pbr_lighting;
API extern const char *skybox_vert_shader_source;
API extern const char *spheremap_skybox_frag_shader_source;
API extern const char *cubemap_skybox_frag_shader_source;
API extern const char *irradiance_frag_shader_source;
API extern const char *importance_sample_ggx;
API extern const char *reflectance_frag_shader_source;
API extern const char *fullscreen_pass_vert_shader_source;
API extern const char *brdf_integration_frag_shader_source;

API extern const char *frag_shader_source;
API extern const char *vert_shader_source;
API extern const char *textured_frag_shader_source;

#endif

// ----------------------------------------------------------------------------

#ifdef PBR_C
#pragma once

const char *preamble = 
"#version 400\n"
"#extension GL_ARB_shading_language_420pack : enable /*layout(binding)*/\n"
"#extension GL_ARB_separate_shader_objects : enable /*layout(location)*/\n"
"const float pi = 3.14159265359, tau = 6.28318530718;\n"
"float dotp(vec3 a, vec3 b) { return max(dot(a,b),0); }\n"
"float pow2(float x) { return x*x; }\n"
"float length2(vec3 v) { return dot(v,v); }\n"
"\n"
"// Our physically based lighting equations use the following common terminology\n"
"// N - normal vector, unit vector perpendicular to the surface\n"
"// V - view vector, unit vector pointing from the surface towards the viewer\n"
"// L - light vector, unit vector pointing from the surface towards the light source\n"
"// H - half-angle vector, unit vector halfway between V and L\n"
"// R - reflection vector, V mirrored about N\n"
"// F0 - base reflectance of the surface\n"
"// alpha - common measure of surface roughness\n"
"float roughness_to_alpha(float roughness) { return roughness*roughness; }\n"
"float trowbridge_reitz_ggx(vec3 N, vec3 H, float alpha) { return alpha*alpha / (pi * pow2(dotp(N,H)*dotp(N,H)*(alpha*alpha-1) + 1)); }\n"
"float geometry_schlick_ggx(vec3 N, vec3 V, float k) { return dotp(N,V) / (dotp(N,V)*(1-k) + k); }\n"
"float geometry_smith(vec3 N, vec3 V, vec3 L, float k) { return geometry_schlick_ggx(N, L, k) * geometry_schlick_ggx(N, V, k); }\n"
"vec3 fresnel_schlick(vec3 V, vec3 H, vec3 F0) { return F0 + (1-F0) * pow(1-dotp(V,H), 5); }\n"
"vec3 cook_torrance(vec3 N, vec3 V, vec3 L, vec3 H, vec3 albedo, vec3 F0, float alpha, float metalness)\n"
"{\n"
"    /*const*/ float D       = trowbridge_reitz_ggx(N, H, alpha);\n"
"    /*const*/ float G       = geometry_smith(N, V, L, (alpha+1)*(alpha+1)/8);\n"
"    /*const*/ vec3 F        = fresnel_schlick(V, H, F0);\n"
"    /*const*/ vec3 diffuse  = (1-F) * (1-metalness) * albedo/pi;\n"
"    /*const*/ vec3 specular = (D * G * F) / (4 * dotp(N,V) * dotp(N,L) + 0.001);  \n"
"    return (diffuse + specular) * dotp(N,L);\n"
"}\n"
"\n"
"vec3 spherical(float phi, float cos_theta, float sin_theta) { return vec3(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta); };\n"
"vec3 spherical(float phi, float theta) { return spherical(phi, cos(theta), sin(theta)); }\n"
"mat3 tangent_basis(vec3 z_direction)\n"
"{\n"
"    /*const*/ vec3 z = normalize(z_direction);\n"
"    /*const*/ vec3 x = normalize(cross(abs(z.z) < 0.999 ? vec3(0,0,1) : vec3(1,0,0), z));\n"
"    /*const*/ vec3 y = cross(z, x);\n"
"    return mat3(x, y, z);\n"
"}\n";

const char *pbr_lighting =
"// This function computes the full lighting to apply to a single fragment\n"
"uniform vec3 u_eye_position;\n"
"layout(binding=0) uniform sampler2D u_brdf_integration_map;\n"
"layout(binding=1) uniform samplerCube u_irradiance_map;\n"
"layout(binding=2) uniform samplerCube u_reflectance_map;\n"
"/*const*/ float MAX_REFLECTANCE_LOD = 4.0;\n"
"vec3 compute_lighting(vec3 position, vec3 normal, vec3 albedo, float roughness, float metalness, float ambient_occlusion)\n"
"{\n"
"    // Compute common terms of lighting equations\n"
"    /*const*/ vec3 N = normalize(normal);\n"
"    /*const*/ vec3 V = normalize(u_eye_position - position);\n"
"    /*const*/ vec3 R = reflect(-V, N);\n"
"    /*const*/ vec3 F0 = mix(vec3(0.04), albedo, metalness);\n"
"    /*const*/ float alpha = roughness_to_alpha(roughness);\n"
"\n"
"    // Initialize our light accumulator\n"
"    vec3 light = vec3(0,0,0);\n"
"\n"
"    // Add contribution from indirect lights\n"
"    {\n"
"        vec2 brdf = texture(u_brdf_integration_map, vec2(dotp(N,V), roughness)).xy;\n"
"        vec3 F    = F0 + max(1-F0-roughness, 0) * pow(1-dotp(N,V), 5);\n"
"        vec3 spec = (F * brdf.x + brdf.y) * textureLod(u_reflectance_map, R, roughness * MAX_REFLECTANCE_LOD).rgb;\n"
"        vec3 diff = (1-F) * (1-metalness) * albedo * texture(u_irradiance_map, N).rgb;\n"
"        light     += (diff + spec) * ambient_occlusion; \n"
"    }\n"
"\n"
"    // Add contributions from direct lights\n"
"    /*const*/ vec3 light_positions[4] = {vec3(-3, -3, 8), vec3(3, -3, 8), vec3(3, 3, 8), vec3(-3, 3, 8)};\n"
"    /*const*/ vec3 light_colors[4] = {vec3(23.47, 21.31, 20.79), vec3(23.47, 21.31, 20.79), vec3(23.47, 21.31, 20.79), vec3(23.47, 21.31, 20.79)};\n"
"    for(int i=0; i<4; ++i)\n"
"    {\n"
"        // Evaluate light vector, half-angle vector, and radiance of light source at the current distance\n"
"        /*const*/ vec3 L = normalize(light_positions[i] - position);\n"
"        /*const*/ vec3 H = normalize(V + L);\n"
"        /*const*/ vec3 radiance = light_colors[i] / length2(light_positions[i] - position);\n"
"        light += radiance * cook_torrance(N, V, L, H, albedo, F0, alpha, metalness);\n"
"    }\n"
"    return light;\n"
"}\n";

const char *skybox_vert_shader_source =
"uniform mat4 u_view_proj_matrix;\n"
"layout(location=0) in vec3 v_direction;\n"
"layout(location=0) out vec3 direction;\n"
"void main()\n"
"{\n"
"    direction   = v_direction;\n"
"    gl_Position = u_view_proj_matrix * vec4(direction,1);\n"
"}\n";

const char *spheremap_skybox_frag_shader_source =
"uniform sampler2D u_texture;\n"
"layout(location=0) in vec3 direction;\n"
"layout(location=0) out vec4 f_color;\n"
"vec2 compute_spherical_texcoords(vec3 direction)\n"
"{\n"
"    return vec2(atan(direction.x, direction.z)*0.1591549, asin(direction.y)*0.3183099 + 0.5);\n"
"}\n"
"void main()\n"
"{\n"
"    f_color = texture(u_texture, compute_spherical_texcoords(normalize(direction)));\n"
"}\n";

const char *cubemap_skybox_frag_shader_source =
"uniform samplerCube u_texture;\n"
"layout(location=0) in vec3 direction;\n"
"layout(location=0) out vec4 f_color;\n"
"void main()\n"
"{\n"
"    f_color = textureLod(u_texture, direction, 1.2);\n"
"}\n";

const char *irradiance_frag_shader_source =
"uniform samplerCube u_texture;\n"
"layout(location=0) in vec3 direction;\n"
"layout(location=0) out vec4 f_color;\n"
"void main()\n"
"{\n"
"    /*const*/ mat3 basis = tangent_basis(direction);\n"
"\n"
"    vec3 irradiance = vec3(0,0,0);\n"
"    float num_samples = 0; \n"
"    for(float phi=0; phi<tau; phi+=0.01)\n"
"    {\n"
"        for(float theta=0; theta<tau/4; theta+=0.01)\n"
"        {\n"
"            // Sample irradiance from the source texture, and weight by the sampling area\n"
"            vec3 L = basis * spherical(phi, theta);\n"
"            irradiance += texture(u_texture, L).rgb * cos(theta) * sin(theta);\n"
"            ++num_samples;\n"
"        }\n"
"    }\n"
"    f_color = vec4(irradiance*(pi/num_samples), 1);\n"
"}\n";

const char *importance_sample_ggx =
"vec3 importance_sample_ggx(float alpha, uint i, uint n)\n"
"{\n"
"    // Phi is distributed uniformly over the integration range\n"
"    /*const*/ float phi = i*tau/n;\n"
"\n"
"    // Theta is importance-sampled using the Van Der Corpus sequence\n"
"    i = (i << 16u) | (i >> 16u);\n"
"    i = ((i & 0x55555555u) << 1u) | ((i & 0xAAAAAAAAu) >> 1u);\n"
"    i = ((i & 0x33333333u) << 2u) | ((i & 0xCCCCCCCCu) >> 2u);\n"
"    i = ((i & 0x0F0F0F0Fu) << 4u) | ((i & 0xF0F0F0F0u) >> 4u);\n"
"    i = ((i & 0x00FF00FFu) << 8u) | ((i & 0xFF00FF00u) >> 8u);\n"
"    float radical_inverse = float(i) /*@r-lyeh*/ * 2.3283064365386963e-10; // Divide by 0x100000000\n"
"    float cos_theta = sqrt((1 - radical_inverse) / ((alpha*alpha-1)*radical_inverse + 1));\n"
"    return spherical(phi, cos_theta, sqrt(1 - cos_theta*cos_theta));\n"
"}\n";

const char *reflectance_frag_shader_source =
"uniform samplerCube u_texture;\n"
"uniform float u_roughness;\n"
"layout(location=0) in vec3 direction;\n"
"layout(location=0) out vec4 f_color;\n"
"\n"
"/*const*/ int sample_count = 1024;\n"
"void main()\n"
"{\n"
"    // As we are evaluating base reflectance, both the normal and view vectors are equal to our sampling direction\n"
"    /*const*/ vec3 N = normalize(direction), V = N;\n"
"    /*const*/ mat3 basis = tangent_basis(N);\n"
"    /*const*/ float alpha = roughness_to_alpha(u_roughness);\n"
"\n"
"    // Precompute the average solid angle of a cube map texel\n"
"    /*const*/ int cube_width = textureSize(u_texture, 0).x;\n"
"    /*const*/ float texel_solid_angle = pi*4 / (6*cube_width*cube_width);\n"
"\n"
"    vec3 sum_color = vec3(0,0,0);\n"
"    float sum_weight = 0;     \n"
"    for(int i=0; i<sample_count; ++i)\n"
"    {\n"
"        // For the desired roughness, sample possible half-angle vectors, and compute the lighting vector from them\n"
"        /*const*/ vec3 H = basis * importance_sample_ggx(alpha, i, sample_count);\n"
"        /*const*/ vec3 L = normalize(2*dot(V,H)*H - V);\n"
"        if(dot(N, L) <= 0) continue;\n"
"\n"
"        // Compute the mip-level at which to sample\n"
"        /*const*/ float D = trowbridge_reitz_ggx(N, H, alpha);\n"
"        /*const*/ float pdf = D*dotp(N,H) / (4*dotp(V,H)) + 0.0001; \n"
"        /*const*/ float sample_solid_angle = 1 / (sample_count * pdf + 0.0001);\n"
"        /*const*/ float mip_level = alpha > 0 ? log2(sample_solid_angle / texel_solid_angle)/2 : 0;\n"
"\n"
"        // Sample the environment map according to the lighting direction, and weight the resulting contribution by N dot L\n"
"        sum_color += textureLod(u_texture, L, mip_level).rgb * dot(N, L);\n"
"        sum_weight += dot(N, L);\n"
"    }\n"
"\n"
"    f_color = vec4(sum_color/sum_weight, 1);\n"
"}\n";

const char *fullscreen_pass_vert_shader_source =
"layout(location=0) in vec2 v_position;\n"
"layout(location=1) in vec2 v_texcoords;\n"
"layout(location=0) out vec2 texcoords;\n"
"void main()\n"
"{\n"
"    texcoords = v_texcoords;\n"
"    gl_Position = vec4(v_position,0,1);\n"
"}\n";

const char *brdf_integration_frag_shader_source =
"layout(location=0) in vec2 texcoords;\n"
"layout(location=0) out vec4 f_color;\n"
"\n"
"/*const*/ int sample_count = 1024;\n"
"vec2 integrate_brdf(float n_dot_v, float alpha)\n"
"{\n"
"    // Without loss of generality, evaluate the case where the normal is aligned with the z-axis and the viewing direction is in the xz-plane\n"
"    /*const*/ vec3 N = vec3(0,0,1);\n"
"    /*const*/ vec3 V = vec3(sqrt(1 - n_dot_v*n_dot_v), 0, n_dot_v);\n"
"\n"
"    vec2 result = vec2(0,0);    \n"
"    for(int i=0; i<sample_count; ++i)\n"
"    {\n"
"        // For the desired roughness, sample possible half-angle vectors, and compute the lighting vector from them\n"
"        /*const*/ vec3 H = importance_sample_ggx(alpha, i, sample_count);\n"
"        /*const*/ vec3 L = normalize(2 * dot(V, H) * H - V);\n"
"        if(dot(N, L) <= 0) continue;\n"
"\n"
"        // Integrate results\n"
"        /*const*/ float Fc = pow(1 - dotp(V,H), 5);\n"
"        /*const*/ float G = geometry_smith(N, V, L, alpha*alpha/2);\n"
"        /*const*/ float G_Vis = (G * dotp(V,H)) / (dotp(N,H) * n_dot_v);\n"
"        result.x += (1 - Fc) * G_Vis;\n"
"        result.y += Fc * G_Vis;\n"
"    }\n"
"    return result/sample_count;\n"
"}\n"
"void main() \n"
"{\n"
"    f_color = vec4(integrate_brdf(texcoords.x, roughness_to_alpha(texcoords.y)), 0, 1);\n"
"}\n";

// ----------------------------------------------------------------------------

const char *vert_shader_source =
"uniform mat4 u_view_proj_matrix;\n"
"uniform mat4 u_model_matrix;\n"
"layout(location=0) in vec3 v_position;\n"
"layout(location=1) in vec3 v_normal;\n"
"layout(location=2) in vec2 v_texcoord;\n"
"layout(location=3) in vec3 v_tangent;\n"
"layout(location=4) in vec3 v_bitangent;\n"
"layout(location=0) out vec3 position;\n"
"layout(location=1) out vec3 normal;\n"
"layout(location=2) out vec2 texcoord;\n"
"layout(location=3) out vec3 tangent;\n"
"layout(location=4) out vec3 bitangent;\n"
"void main() {\n"
"    position    = (u_model_matrix * vec4(v_position,1)).xyz;\n"
"    normal      = normalize((u_model_matrix * vec4(v_normal,0)).xyz);\n"
"    texcoord    = v_texcoord;\n"
"    tangent     = normalize((u_model_matrix * vec4(v_tangent,0)).xyz);\n"
"    bitangent   = normalize((u_model_matrix * vec4(v_bitangent,0)).xyz);\n"
"    gl_Position = u_view_proj_matrix * vec4(position,1);\n"
"}\n";

const char *frag_shader_source =
"// Fragment shader for untextured PBR surface\n"
"uniform vec3 u_albedo;\n"
"uniform float u_roughness;\n"
"uniform float u_metalness;\n"
"uniform float u_ambient_occlusion;\n"
"layout(location=0) in vec3 position;\n"
"layout(location=1) in vec3 normal;\n"
"layout(location=0) out vec4 f_color;\n"
"void main() {\n"
"    // Compute our PBR lighting\n"
"    vec3 light = compute_lighting(position, normal, u_albedo, u_roughness, u_metalness, u_ambient_occlusion);\n"
"\n"
"    // Apply simple tone mapping and write to fragment\n"
"    f_color = vec4(light / (light + 1), 1);\n"
"}\n";

const char *textured_frag_shader_source =
"// Fragment shader for textured PBR surface\n"
"layout(binding=3) uniform sampler2D u_albedo_tex;\n"
"layout(binding=4) uniform sampler2D u_normal_tex;\n"
"layout(binding=5) uniform sampler2D u_roughness_tex;\n"
"layout(binding=6) uniform sampler2D u_metalness_tex;\n"
"uniform float u_ambient_occlusion;\n"
"layout(location=0) in vec3 position;\n"
"layout(location=1) in vec3 normal;\n"
"layout(location=2) in vec2 texcoord;\n"
"layout(location=3) in vec3 tangent;\n"
"layout(location=4) in vec3 bitangent;\n"
"layout(location=0) out vec4 f_color;\n"
"void main() {\n"
"    // Compute our PBR lighting\n"
"    vec3 ts_normal = texture(u_normal_tex, texcoord).xyz * 2 - 1;\n"
"    vec3 ws_normal = normalize(tangent) * ts_normal.x + normalize(bitangent) * ts_normal.y + normalize(normal) * ts_normal.z;\n"
"    vec3 light = compute_lighting(position, ws_normal, texture(u_albedo_tex, texcoord).rgb, texture(u_roughness_tex, texcoord).r, texture(u_metalness_tex, texcoord).g, u_ambient_occlusion);\n"
"\n"
"    // Apply simple tone mapping and write to fragment\n"
"    f_color = vec4(light / (light + 1), 1);\n"
"}\n";

// ----------------------------------------------------------------------------


pbr_tools make_pbr() {
    pbr_tools p = {0};
    GLuint skybox_vs             = compile_shader('v', 2, preamble, skybox_vert_shader_source);
    GLuint fullscreen_pass_vs    = compile_shader('v', 2, preamble, fullscreen_pass_vert_shader_source);
    p.spheremap_skybox_prog      = make_program(skybox_vs, compile_shader('f', 2, preamble, spheremap_skybox_frag_shader_source));
    p.cubemap_skybox_prog        = make_program(skybox_vs, compile_shader('f', 2, preamble, cubemap_skybox_frag_shader_source));
    p.irradiance_prog            = make_program(skybox_vs, compile_shader('f', 2, preamble, irradiance_frag_shader_source));
    p.reflectance_prog           = make_program(skybox_vs, compile_shader('f', 3, preamble, importance_sample_ggx, reflectance_frag_shader_source));
    p.brdf_integration_prog      = make_program(fullscreen_pass_vs, compile_shader('f', 3, preamble, importance_sample_ggx, brdf_integration_frag_shader_source));

    vec3 vertices[] = {{+1,-1,+1},{+1,+1,+1},{+1,+1,-1},{-1,+1,-1},{+1,-1,-1},{-1,-1,-1},{-1,-1,+1},{-1,+1,+1}};
    unsigned indices[] = { 0, 1, 2, 3, 4, 5, 6, 3, 7, 1, 6, 0, 4, 2 };
    mesh_create(&p.skybox, "p3", 0,countof(vertices),vertices, countof(indices),indices, MESH2_TRIANGLE_STRIP);

    return p;
}

GLuint pbr_compute_brdfmap(pbr_tools *pbr) {
    GLuint brdf_integration_map;
    glCreateTextures(GL_TEXTURE_2D, 1, &brdf_integration_map);
    glTextureStorage2D(brdf_integration_map, 1, GL_RG16F, 512, 512);
    glTextureParameteri(brdf_integration_map, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(brdf_integration_map, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(brdf_integration_map, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(brdf_integration_map, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint fbo;
    glCreateFramebuffers(1, &fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdf_integration_map, 0);
    glViewport(0,0,512,512);

    use(pbr->brdf_integration_prog);

    glBegin(GL_QUADS);
    glVertexAttrib2f(1, 0, 0); glVertex2f(-1, -1);
    glVertexAttrib2f(1, 0, 1); glVertex2f(-1, +1);
    glVertexAttrib2f(1, 1, 1); glVertex2f(+1, +1);
    glVertexAttrib2f(1, 1, 0); glVertex2f(+1, -1);
    glEnd();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);

    return brdf_integration_map;
}

GLuint render_cubemap(GLsizei levels, GLenum internal_format, GLsizei width, void (*draw_face)(mat44 view_proj_matrix, int mip) ) {
    float face0[] = {0,0,+1,0, 0,+1,0,0, -1,0,0,0, 0,0,0,1};
    float face1[] = {0,0,-1,0, 0,+1,0,0, +1,0,0,0, 0,0,0,1};
    float face2[] = {+1,0,0,0, 0,0,-1,0, 0,+1,0,0, 0,0,0,1};
    float face3[] = {+1,0,0,0, 0,0,+1,0, 0,-1,0,0, 0,0,0,1};
    float face4[] = {+1,0,0,0, 0,+1,0,0, 0,0,+1,0, 0,0,0,1};
    float face5[] = {-1,0,0,0, 0,+1,0,0, 0,0,-1,0, 0,0,0,1};

    // If user passes levels == 0, let the user draw level 1 and have OpenGL generate a mip chain
    GLsizei user_levels = levels ? levels : 1;

    GLuint cubemap;
    glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &cubemap);
    glTextureStorage2D(cubemap, levels ? levels : 1+((GLsizei)ceilf(log2f(width))), internal_format, width, width);
    glTextureParameteri(cubemap, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(cubemap, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(cubemap, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTextureParameteri(cubemap, GL_TEXTURE_MIN_FILTER, levels == 1 ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(cubemap, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLuint fbo;
    glCreateFramebuffers(1, &fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    for(GLint mip=0; mip<user_levels; ++mip)
    {
        glViewport(0, 0, width, width);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, cubemap, mip); draw_face(face0, mip);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, cubemap, mip); draw_face(face1, mip);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, cubemap, mip); draw_face(face2, mip);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, cubemap, mip); draw_face(face3, mip);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, cubemap, mip); draw_face(face4, mip);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, cubemap, mip); draw_face(face5, mip);
        width = (width/2) > 1 ? (width/2) : 1;
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);

    if(levels == 0) glGenerateTextureMipmap(cubemap);
    return cubemap; 
}

static THREAD_LOCAL const pbr_tools *g_pbr = 0;
static THREAD_LOCAL GLuint g_spheremap = 0, g_cubemap = 0;

static
void pbr_convert_spheremap2cubemap_(mat44 view_proj_matrix, int mip) {
    use(g_pbr->spheremap_skybox_prog);
    bind_texture(g_pbr->spheremap_skybox_prog, "u_texture", g_spheremap);
    uniform16f(g_pbr->spheremap_skybox_prog, "u_view_proj_matrix", view_proj_matrix);
    mesh_render((mesh*)&g_pbr->skybox, 0);
}

GLuint pbr_convert_spheremap2cubemap(const pbr_tools *pbr, GLenum internal_format, GLsizei width, GLuint spheremap) {
    g_pbr = pbr;
    g_spheremap = spheremap;
    return render_cubemap(0, internal_format, width, pbr_convert_spheremap2cubemap_);
}

static
void pbr_compute_irradiancemap_(mat44 view_proj_matrix, int mip) {
    use(g_pbr->irradiance_prog);
    bind_texture(g_pbr->irradiance_prog, "u_texture", g_cubemap);
    uniform16f(g_pbr->irradiance_prog, "u_view_proj_matrix", view_proj_matrix);
    mesh_render((mesh*)&g_pbr->skybox, 0);
}

GLuint pbr_compute_irradiancemap(const pbr_tools *pbr, GLuint cubemap) {
    g_pbr = pbr;
    g_cubemap = cubemap;
    return render_cubemap(1, GL_RGB16F, 32, pbr_compute_irradiancemap_);
}

static
void pbr_compute_reflectancemap_(mat44 view_proj_matrix, int mip) {
    use(g_pbr->reflectance_prog);
    bind_texture(g_pbr->reflectance_prog, "u_texture", g_cubemap);
    uniform16f(g_pbr->reflectance_prog, "u_view_proj_matrix", view_proj_matrix);
    uniform1f(g_pbr->reflectance_prog, "u_roughness", mip/4.0f);
    mesh_render((mesh*)&g_pbr->skybox, 0);
}

GLuint pbr_compute_reflectancemap(const pbr_tools *pbr, GLuint cubemap) {
    g_pbr = pbr;
    g_cubemap = cubemap;
    return render_cubemap(5, GL_RGB16F, 128, pbr_compute_reflectancemap_);
}

void pbr_draw_skybox(const pbr_tools *pbr, GLuint cubemap, mat44 skybox_view_proj_matrix) {
    use(pbr->cubemap_skybox_prog);
    bind_texture(pbr->cubemap_skybox_prog, "u_texture", cubemap);
    uniform16f(pbr->cubemap_skybox_prog, "u_view_proj_matrix", skybox_view_proj_matrix);
    mesh_render((mesh*)&pbr->skybox, 0);
}

environment load_enviroment(const pbr_tools *tools, const char *filename) {
    // Load spheremap
    int width, height;
    stbi_set_flip_vertically_on_load(1);
    float *pixels = stbi_loadf(filename, &width, &height, NULL, 3);
    if( !pixels ) PANIC("cannot load file %s", filename);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    stbi_image_free(pixels);

    printf("Loaded %s (%dx%d)\n", filename, width, height);

    // Compute environment maps
    const GLuint environment_cubemap = pbr_convert_spheremap2cubemap(tools, GL_RGB16F, 1024, tex);
    const GLuint irradiance_cubemap = pbr_compute_irradiancemap(tools, environment_cubemap);
    const GLuint reflectance_cubemap = pbr_compute_reflectancemap(tools, environment_cubemap);
    glDeleteTextures(1, &tex);
    environment env = {environment_cubemap, irradiance_cubemap, reflectance_cubemap};
    return env;
}

#endif

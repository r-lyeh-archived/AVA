#version 330
#extension GL_ARB_shader_precision          : enable
#extension GL_ARB_conservative_depth        : enable
#extension GL_ARB_texture_cube_map_array    : enable
#extension GL_ARB_shading_language_420pack  : enable
#extension GL_ARB_shading_language_packing  : enable
#extension GL_ARB_explicit_uniform_location : enable
layout(depth_unchanged) out float gl_FragDepth;

layout(binding = 9) uniform samplerCubeArray in_cubemaps;

smooth in vec3 vs_position;

out vec4 fs_color;

void main()
{
  fs_color = texture(in_cubemaps, vec4(vs_position, 0));
}

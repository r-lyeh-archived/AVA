#version 330
#extension GL_ARB_shader_precision          : enable
#extension GL_ARB_conservative_depth        : enable
#extension GL_ARB_texture_cube_map_array    : enable
#extension GL_ARB_shading_language_420pack  : enable
#extension GL_ARB_shading_language_packing  : enable
#extension GL_ARB_explicit_uniform_location : enable
layout(depth_unchanged) out float gl_FragDepth;

vec4 when_eq  (vec4 x, vec4 y) { return 1.0 - abs(sign(x - y)); }
vec4 when_neq (vec4 x, vec4 y) { return abs(sign(x - y)); }
vec4 when_gt  (vec4 x, vec4 y) { return max(sign(x - y), 0); }
vec4 when_lt  (vec4 x, vec4 y) { return max(sign(y - x), 0); }
vec4 when_ge  (vec4 x, vec4 y) { return 1.0 - when_lt(x, y); }
vec4 when_le  (vec4 x, vec4 y) { return 1.0 - when_gt(x, y); }

vec3 IntToColor(int i)
{
  vec3 color;
  color.r = float(i >> 16 & 0xFF) / 255.0;
  color.g = float(i >> 8  & 0xFF) / 255.0;
  color.b = float(i       & 0xFF) / 255.0;
  return color;
}

layout(binding = 8) uniform sampler2DArray   in_textures;
layout(binding = 9) uniform samplerCubeArray in_cubemaps;

layout(location = 0) uniform vec3 cam_pos;
layout(location = 1) uniform int show_pass;

layout(location = 0) out vec4 fbo_color;

flat   in int  vs_id;
smooth in vec2 vs_uv;
smooth in vec3 vs_normal;
smooth in vec3 vs_position;

void main()
{
  vec4 diffuse = texture(in_textures, vec3(vs_uv, vs_id));
  vec4 reflect = texture(in_cubemaps, vec4(reflect((vs_position - cam_pos), vs_normal), 0));
  vec4 colours = vec4(0);
  
  colours += vec4(1.0, 0.0, 0.0, 1.0) * when_eq(vec4(vs_id), vec4(0));
  colours += vec4(0.0, 1.0, 0.0, 1.0) * when_eq(vec4(vs_id), vec4(1));
  colours += vec4(0.0, 0.0, 1.0, 1.0) * when_eq(vec4(vs_id), vec4(2));
  
  fbo_color = mix((diffuse * 0.4) + (colours * 0.6), reflect, dot(vs_normal, normalize(vs_position - cam_pos)) * 0.5 + 0.5);
  
  if(show_pass == 1) fbo_color = diffuse;
  if(show_pass == 2) fbo_color = reflect;
  if(show_pass == 3) fbo_color = colours;
  if(show_pass == 4) fbo_color = vec4(IntToColor(vs_id + 1), 1);
}

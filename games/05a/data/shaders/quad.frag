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

int ColorToInt(vec3 c)
{
  int r = int(c.r * 255.0);
  int g = int(c.g * 255.0);
  int b = int(c.b * 255.0);
  return b + (g << 8) + (r << 16);
}

vec3 ScreenSpaceDither(vec2 vScreenPos)
{
  vec3 vDither = vec3(dot(vec2(171.0, 231.0), vScreenPos.xy));
  vDither.rgb = fract(vDither.rgb / vec3(103.0, 71.0, 97.0)) - vec3(0.5, 0.5, 0.5);
  return vDither.rgb / 255.0;
}

layout(binding = 10) uniform sampler2DArray in_fbo_color;

smooth in vec2 vs_uv;
out vec4 color;

void main()
{
  color = texture(in_fbo_color, vec3(vs_uv, 0));
  
  color += vec4(ScreenSpaceDither(gl_FragCoord.xy), 0);
}

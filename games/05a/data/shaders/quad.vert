#version 330
#extension GL_ARB_shader_precision          : enable
#extension GL_ARB_shading_language_420pack  : enable
#extension GL_ARB_explicit_uniform_location : enable
out gl_PerVertex { vec4 gl_Position; };

const vec2 pos[] = vec2[]
(
  vec2( -1.0, -1.0 ),
  vec2(  1.0, -1.0 ),
  vec2( -1.0,  1.0 ),
  vec2( -1.0,  1.0 ),
  vec2(  1.0, -1.0 ),
  vec2(  1.0,  1.0 )
);

const vec2 uv[] = vec2[]
(
  vec2(  0.0,  0.0 ),
  vec2(  1.0,  0.0 ),
  vec2(  0.0,  1.0 ),
  vec2(  0.0,  1.0 ),
  vec2(  1.0,  0.0 ),
  vec2(  1.0,  1.0 )
);

smooth out vec2 vs_uv;

void main()
{
  gl_Position = vec4(pos[gl_VertexID], 0, 1);
  vs_uv = uv[gl_VertexID];
}

#version 330
#extension GL_ARB_shader_precision          : enable
#extension GL_ARB_shading_language_420pack  : enable
#extension GL_ARB_explicit_uniform_location : enable
out gl_PerVertex { vec4 gl_Position; };

const vec3 pos[] = vec3[]
(
  vec3( -1.0,  1.0, -1.0 ),
  vec3( -1.0, -1.0, -1.0 ),
  vec3(  1.0, -1.0, -1.0 ),
  vec3(  1.0, -1.0, -1.0 ),
  vec3(  1.0,  1.0, -1.0 ),
  vec3( -1.0,  1.0, -1.0 ),

  vec3( -1.0, -1.0,  1.0 ),
  vec3( -1.0, -1.0, -1.0 ),
  vec3( -1.0,  1.0, -1.0 ),
  vec3( -1.0,  1.0, -1.0 ),
  vec3( -1.0,  1.0,  1.0 ),
  vec3( -1.0, -1.0,  1.0 ),

  vec3(  1.0, -1.0, -1.0 ),
  vec3(  1.0, -1.0,  1.0 ),
  vec3(  1.0,  1.0,  1.0 ),
  vec3(  1.0,  1.0,  1.0 ),
  vec3(  1.0,  1.0, -1.0 ),
  vec3(  1.0, -1.0, -1.0 ),

  vec3( -1.0, -1.0,  1.0 ),
  vec3( -1.0,  1.0,  1.0 ),
  vec3(  1.0,  1.0,  1.0 ),
  vec3(  1.0,  1.0,  1.0 ),
  vec3(  1.0, -1.0,  1.0 ),
  vec3( -1.0, -1.0,  1.0 ),

  vec3( -1.0,  1.0, -1.0 ),
  vec3(  1.0,  1.0, -1.0 ),
  vec3(  1.0,  1.0,  1.0 ),
  vec3(  1.0,  1.0,  1.0 ),
  vec3( -1.0,  1.0,  1.0 ),
  vec3( -1.0,  1.0, -1.0 ),

  vec3( -1.0, -1.0, -1.0 ),
  vec3( -1.0, -1.0,  1.0 ),
  vec3(  1.0, -1.0, -1.0 ),
  vec3(  1.0, -1.0, -1.0 ),
  vec3( -1.0, -1.0,  1.0 ),
  vec3(  1.0, -1.0,  1.0 )
);

vec4 qconj(vec4 q)
{
  return vec4(-q.xyz, q.w);
}

vec3 qrot(vec3 v, vec4 q)
{
  return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + v * q.w);
}

vec4 proj(vec3 mv, vec4 p)
{
  return vec4(mv.xy * p.xy, mv.z * p.z + p.w, -mv.z);
}

layout(location = 0) uniform vec4 cam_rot;
layout(location = 1) uniform vec4 cam_prj;

smooth out vec3 vs_position;

void main()
{
  vec3 mv = pos[gl_VertexID];
  mv = qrot(mv, qconj(cam_rot));
  gl_Position = proj(mv, cam_prj);
  vs_position = pos[gl_VertexID];
}

#version 330
#extension GL_ARB_shader_precision          : enable
#extension GL_ARB_shading_language_420pack  : enable
#extension GL_ARB_explicit_uniform_location : enable
out gl_PerVertex { vec4 gl_Position; };

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

layout(location = 0) uniform vec3 cam_pos;
layout(location = 1) uniform vec4 cam_rot;
layout(location = 2) uniform vec4 cam_prj;

layout(binding = 0) uniform isamplerBuffer in_mesh_id;
layout(binding = 1) uniform isamplerBuffer in_attr_first;
layout(binding = 2) uniform isamplerBuffer in_attr_id;

layout(binding = 3) uniform  samplerBuffer in_pos;
layout(binding = 4) uniform  samplerBuffer in_uv;
layout(binding = 5) uniform  samplerBuffer in_normal;

layout(binding = 6) uniform isamplerBuffer in_ins_first;
layout(binding = 7) uniform  samplerBuffer in_ins_pos;

flat   out int  vs_id;
smooth out vec2 vs_uv;
smooth out vec3 vs_normal;
smooth out vec3 vs_position;

void main()
{
  int   mesh_id    = texelFetch(in_mesh_id, gl_VertexID).x;
  ivec3 attr_first = texelFetch(in_attr_first,  mesh_id).xyz;
  ivec3 attr_id    = texelFetch(in_attr_id, gl_VertexID).xyz;
  
  vec3 pos    = texelFetch(in_pos,    attr_first.x + attr_id.x).xyz;
  vec2 uv     = texelFetch(in_uv,     attr_first.y + attr_id.y).xy;
  vec3 normal = texelFetch(in_normal, attr_first.z + attr_id.z).xyz;
  
  int  ins_first = texelFetch(in_ins_first, mesh_id).x;
  vec3 ins_pos   = texelFetch(in_ins_pos, ins_first + gl_InstanceID).xyz;
  
  vec3 mv;
  mv = pos + ins_pos;
  
  vs_id       = mesh_id;
  vs_uv       = uv;
  vs_normal   = normal;
  vs_position = mv;
  
  mv -= cam_pos;
  mv  = qrot(mv, qconj(cam_rot));
  gl_Position = proj(mv, cam_prj);
}

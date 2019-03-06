// Public domain single-file C implementation by Constantine Tarasenkov
//
// LICENSE
//
// This is free and unencumbered software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
// 
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#ifndef QFPC_H_
#define QFPC_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef QFPC_SIN
#define QFPC_SIN sin
#endif

#ifndef QFPC_COS
#define QFPC_COS cos
#endif

#ifndef QFPC_SQRT
#define QFPC_SQRT sqrt
#endif

#ifndef QFPC_TO_RAD
#define QFPC_TO_RAD 0.0174532925199432957f
#endif

void qfpcQAxisAngle(float * q, const float * axis, float angle_rad);
void qfpcQMulq(float * result, const float * q1, const float * q2);
float qfpcV3Len(const float * v);
void qfpcV3Norm(float * v);
void qfpcV3Mulf(float * result, const float * v, float f);  
void qfpcV3Addv(float * result, const float * v1, const float * v2);
void qfpcV3Cross(float * result, const float * v1, const float * v2);
void qfpcV3Rotq(float * result, const float * v, const float * q);

void quatFirstPersonCamera(
  float * camera_vec3_position,
  float * camera_quat_rotation,
  float * camera_out_matrix3x3,
  float look_multiplier,
  float move_multiplier,
  int mouse_x_delta,
  int mouse_y_delta,
  int key_pressed_forward,
  int key_pressed_left,
  int key_pressed_back,
  int key_pressed_right,
  int key_pressed_up,
  int key_pressed_down
);

#ifdef __cplusplus
}
#endif
#endif // QFPC_H_

#ifdef QFPC_IMPLEMENTATION

void qfpcQAxisAngle(
  float * q,
  const float * axis,
  float angle_rad)
{
  float s = (float)QFPC_SIN(angle_rad / 2.f);
  float c = (float)QFPC_COS(angle_rad / 2.f);
  
  q[0] = axis[0] * s;
  q[1] = axis[1] * s;
  q[2] = axis[2] * s;
  q[3] = c;
}

void qfpcQMulq(
  float * result,
  const float * q1,
  const float * q2)
{
  float q1x = q1[0];
  float q1y = q1[1];
  float q1z = q1[2];
  float q1w = q1[3];
  
  float q2x = q2[0];
  float q2y = q2[1];
  float q2z = q2[2];
  float q2w = q2[3];
  
  result[0] = q1x * q2w + q1y * q2z - q1z * q2y + q1w * q2x;
  result[1] = q1x * q2z - q1y * q2w - q1z * q2x - q1w * q2y;
  result[2] = q1x * q2y - q1y * q2x + q1z * q2w + q1w * q2z;
  result[3] = q1x * q2x + q1y * q2y + q1z * q2z - q1w * q2w;
}

float qfpcV3Len(const float * v)
{
  float x = v[0];
  float y = v[1];
  float z = v[2];
  
  return (float)QFPC_SQRT((x * x) + (y * y) + (z * z));
}

void qfpcV3Norm(float * v)
{
  float l = qfpcV3Len(v);
  if(l == 0.f) return;
  
  v[0] /= l;
  v[1] /= l;
  v[2] /= l;
}

void qfpcV3Mulf(
  float * result,
  const float * v,
  float f)
{
  result[0] = v[0] * f;
  result[1] = v[1] * f;
  result[2] = v[2] * f;
}

void qfpcV3Addv(
  float * result,
  const float * v1,
  const float * v2)
{
  result[0] = v1[0] + v2[0];
  result[1] = v1[1] + v2[1];
  result[2] = v1[2] + v2[2];
}

void qfpcV3Cross(
  float * result,
  const float * v1,
  const float * v2)
{
  float v1x = v1[0];
  float v1y = v1[1];
  float v1z = v1[2];
  
  float v2x = v2[0];
  float v2y = v2[1];
  float v2z = v2[2];
  
  result[0] = (v1y * v2z) - (v1z * v2y);
  result[1] = (v1z * v2x) - (v1x * v2z);
  result[2] = (v1x * v2y) - (v1y * v2x);
}

void qfpcV3Rotq(
  float * result,
  const float * v,
  const float * q)
{
  float temp[3];
  qfpcV3Mulf(temp, v, q[3]);
  qfpcV3Cross(result, q, v);
  qfpcV3Addv(result, result, temp);
  qfpcV3Cross(result, q, result);
  qfpcV3Mulf(result, result, 2.f);
  qfpcV3Addv(result, result, v);
}

void quatFirstPersonCamera(
  float * cam_vec3,
  float * cam_quat,
  float * cam_m3x3,
  float look_mult,
  float move_mult,
  int x_delta,
  int y_delta,
  int key_f,
  int key_l,
  int key_b,
  int key_r,
  int key_u,
  int key_d)
{
  float x_quat[4];
  float y_quat[4];
  
  const float nx[3] = { -1.f,  0.f,  0.f };
  const float py[3] = {  0.f,  1.f,  0.f };
  const float ny[3] = {  0.f, -1.f,  0.f };
  const float nz[3] = {  0.f,  0.f, -1.f };
  
  qfpcQAxisAngle(y_quat, nx, (y_delta * look_mult) * QFPC_TO_RAD);
  qfpcQAxisAngle(x_quat, ny, (x_delta * look_mult) * QFPC_TO_RAD);
  
  qfpcQMulq(cam_quat, cam_quat, y_quat);
  qfpcQMulq(cam_quat, x_quat, cam_quat);
  
  float sd_vec3[3];
  float up_vec3[3];
  float fw_vec3[3];
  
  qfpcV3Rotq(sd_vec3, nx, cam_quat);
  qfpcV3Rotq(up_vec3, py, cam_quat);
  qfpcV3Rotq(fw_vec3, nz, cam_quat);
  
  cam_m3x3[0] = -sd_vec3[0];
  cam_m3x3[1] =  up_vec3[0];
  cam_m3x3[2] = -fw_vec3[0];
  
  cam_m3x3[3] = -sd_vec3[1];
  cam_m3x3[4] =  up_vec3[1];
  cam_m3x3[5] = -fw_vec3[1];
  
  cam_m3x3[6] = -sd_vec3[2];
  cam_m3x3[7] =  up_vec3[2];
  cam_m3x3[8] = -fw_vec3[2];
  
  qfpcV3Mulf(sd_vec3, sd_vec3, (float)(key_l - key_r));
  qfpcV3Mulf(up_vec3, up_vec3, (float)(key_u - key_d));
  qfpcV3Mulf(fw_vec3, fw_vec3, (float)(key_f - key_b));
  
  float move_vec3[3] = { 0.f };
  
  qfpcV3Addv(move_vec3, move_vec3, sd_vec3);
  qfpcV3Addv(move_vec3, move_vec3, up_vec3);
  qfpcV3Addv(move_vec3, move_vec3, fw_vec3);
  qfpcV3Norm(move_vec3);
  qfpcV3Mulf(move_vec3, move_vec3, move_mult);
  
  qfpcV3Addv(cam_vec3, cam_vec3, move_vec3);
}

#endif // QFPC_IMPLEMENTATION

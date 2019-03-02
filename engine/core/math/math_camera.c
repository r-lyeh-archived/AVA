// flycamera code by Constantine Tarasenkov (unlicensed, see license at bottom of file).
// flycamera source by @nlguillemot (public domain).

void flycamera(
    float * cam_m4x4,
    float * cam_vec3, float * cam_quat,  //float * cam_m3x3,
    float look_mult, float move_mult,
    float x_delta, float y_delta,
    float key_f, float key_l, float key_b, float key_r, float key_u, float key_d);

void flycam(
    float view[16],                         // The matrix that will be updated with the new view transform. Previous contents don't matter.
    float eye[3],                           // Current eye position. Will be updated to new eye position.
    float look[3],                          // Current look direction. Will be updated to new look direction.
    float up[3],                            // Camera's "up" direction. Likely (0,1,0). Likely constant throughout application.
    float delta_time_seconds,               // Amount of seconds passed since last update.
    float eye_speed,                        // How much the eye should move in world units per second.
    float degrees_per_cursor_move,          // How many degrees the camera rotates when the mouse moves by that many units.
    float max_pitch_rotation_degrees,       // How far up or down you're allowed to look (0º can't look up or down at all; 80º can look straight up, but not quite)
    int delta_cursor_x, int delta_cursor_y, // Update these every frame based on horizontal and vertical mouse movement.
    int forward_held, int left_held, int backward_held, int right_held, int jump_held, int crouch_held); // Keyboard layout: W,A,S,D, space, ctrl

// cam_position, target_position, up_vector
void lookat_camera(float m[16], float eye[3], float pos[3], float up[3]);

void CreatePerspectiveProjection(float * proj, float aspect, float fov_y_rad, float n, float f);



#ifdef CAMERA_C
#pragma once

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <math.h>

// flycamera code,
// by Constantine Tarasenkov (public domain).

void flycamera(
    float * cam_m4x4,
    float * cam_vec3, float * cam_quat,  //float * cam_m3x3,
    float look_mult, float move_mult,
    float x_delta, float y_delta,
    float key_f, float key_l, float key_b, float key_r, float key_u, float key_d)
{
    // y_quat = quatFromAxisAngle({1, 0, 0}, y_delta * look_mult * to_radians)
    // x_quat = quatFromAxisAngle({0, 1, 0}, x_delta * look_mult * to_radians)

    float ydt = y_delta * look_mult * 0.017453f;
    float xdt = x_delta * look_mult * 0.017453f;
    float yqx = (float)sin(ydt);
    float yqw = (float)cos(ydt);
    float xqy = (float)sin(xdt);
    float xqw = (float)cos(xdt);

    // cam_quat = cam_quat * y_quat
    // cam_quat = x_quat * cam_quat

    float iqx = cam_quat[0];
    float iqy = cam_quat[1];
    float iqz = cam_quat[2];
    float iqw = cam_quat[3];

    float tqx = +iqw * yqx + iqx * yqw;
    float tqy = -iqz * yqx - iqy * yqw;
    float tqz = -iqy * yqx + iqz * yqw;
    float tqw = +iqx * yqx - iqw * yqw;

    float cqx = +tqz * xqy + tqx * xqw;
    float cqy = -tqw * xqy - tqy * xqw;
    float cqz = -tqx * xqy + tqz * xqw;
    float cqw = +tqy * xqy - tqw * xqw;

    // x_axis = rotateVec3ByQuat({-1, 0, 0}, cam_quat)
    // y_axis = rotateVec3ByQuat({ 0, 1, 0}, cam_quat)
    // z_axis = rotateVec3ByQuat({ 0, 0,-1}, cam_quat)

    float qxqx = cqx * cqx;
    float qxqy = cqx * cqy;
    float qxqz = cqx * cqz;
    float qxqw = cqx * cqw;

    float qyqy = cqy * cqy;
    float qyqz = cqy * cqz;
    float qyqw = cqy * cqw;

    float qzqz = cqz * cqz;
    float qzqw = cqz * cqw;

    float xax = (qyqy + qzqz) *  2.f - 1.f;
    float xay = (qxqy + qzqw) * -2.f;
    float xaz = (qyqw - qxqz) *  2.f;

    float yax = (qxqy - qzqw) *  2.f;
    float yay = (qxqx + qzqz) * -2.f + 1.f;
    float yaz = (qxqw + qyqz) *  2.f;

    float zax = (qyqw + qxqz) * -2.f;
    float zay = (qxqw - qyqz) *  2.f;
    float zaz = (qxqx + qyqy) *  2.f - 1.f;

    float xm = key_l - key_r;
    float ym = key_u - key_d;
    float zm = key_f - key_b;

    float mx = xax * xm + yax * ym + zax * zm;
    float my = xay * xm + yay * ym + zay * zm;
    float mz = xaz * xm + yaz * ym + zaz * zm;

    float ln = (float)sqrt(mx * mx + my * my + mz * mz);
    if (ln != 0) {
        mx /= ln;
        my /= ln;
        mz /= ln;
    }

    cam_vec3[0] += mx * move_mult;
    cam_vec3[1] += my * move_mult;
    cam_vec3[2] += mz * move_mult;

    cam_quat[0] =  cqx;
    cam_quat[1] =  cqy;
    cam_quat[2] =  cqz;
    cam_quat[3] =  cqw;

    cam_m4x4[ 0] = -xax;
    cam_m4x4[ 1] =  yax;
    cam_m4x4[ 2] = -zax;
    cam_m4x4[ 3] = 0;

    cam_m4x4[ 4] = -xay;
    cam_m4x4[ 5] =  yay;
    cam_m4x4[ 6] = -zay;
    cam_m4x4[ 7] = 0;

    cam_m4x4[ 8] = -xaz;
    cam_m4x4[ 9] =  yaz;
    cam_m4x4[10] = -zaz;
    cam_m4x4[11] = 0;

    cam_m4x4[12] = 0;
    cam_m4x4[13] = 0;
    cam_m4x4[14] = 0;
    cam_m4x4[15] = 0;
}

// cam_position, target_position, up_vector
void lookat_camera(float m[16], float eye[3], float pos[3], float up[3]) {
    float ln, eps = 0.0001f;

    float f0 = eye[0]-pos[0],     f1 = eye[1]-pos[1],     f2 = eye[2]-pos[2];
    float r0 = up[1]*f2-up[2]*f1, r1 = up[2]*f0-up[0]*f2, r2 = up[0]*f1-up[1]*f0;
    float u0 = f1*r2-f2*r1,       u1 = f2*r0-f0*r2,       u2 = f0*r1-f1*r0;

    // normalize front, right, up vectors

    ln = sqrt( f0 * f0 + f1 * f1 + f2 * f2 );
    if( ln != 0 ) {
        f0 /= ln;
        f1 /= ln;
        f2 /= ln;
    }

    ln = sqrt( r0 * r0 + r1 * r1 + r2 * r2 );
    if( ln != 0 ) {
        r0 /= ln;
        r1 /= ln;
        r2 /= ln;
    }

    ln = sqrt( u0 * u0 + u1 * u1 + u2 * u2 );
    if( ln != 0 ) {
        u0 /= ln;
        u1 /= ln;
        u2 /= ln;
    }

    // column-major order notation (opengl)
    // (m0 .. m12)
    // (   ..    )
    // (m3 .. m15)

    m[0] = r0;  m[4] = r1;  m[ 8] = r2; m[12] = -r0*eye[0] -r1*eye[1] -r2*eye[2];
    m[1] = u0;  m[5] = u1;  m[ 9] = u2; m[13] = -u0*eye[0] -u1*eye[1] -u2*eye[2];
    m[2] = f0;  m[6] = f1;  m[10] = f2; m[14] = -f0*eye[0] -f1*eye[1] -f2*eye[2];
    m[3]=   0;  m[7]=   0;  m[11] =  0; m[15] = 1;
}

// out is vec4
void CreatePerspectiveProjection(float * proj, float aspect, float fov_y_rad, float n, float f) {
    float d = 1.f / (float)tan(fov_y_rad * 0.5f);
    proj[0] = d / aspect;
    proj[1] = d;
    proj[2] = f / (n - f);
    proj[3] = (f * n) / (n - f);
}


#include <math.h>
#include <assert.h>
//#include "linear.c"
#include "../math/math_linear.c"

void flycam(
    float view[16],
    float eye[3],
    float look[3],
    float up[3],
    float delta_time_seconds,
    float eye_speed,
    float degrees_per_cursor_move,
    float max_pitch_rotation_degrees,
    int delta_cursor_x, int delta_cursor_y,
    int forward_held, int left_held, int backward_held, int right_held, int jump_held, int crouch_held)
{
    float look_len = sqrtf(look[0] * look[0] + look[1] * look[1] + look[2] * look[2]);
    float up_len = sqrtf(up[0] * up[0] + up[1] * up[1] + up[2] * up[2]);

    // unit length of look direction is expected and maintained throughout the algorithm
    // otherwise, the vector gets smaller and smaller as error accumulates, eventually becoming 0.
    assert(fabsf(look_len - 1.0f) < 0.000001f);
    assert(fabsf(up_len - 1.0f) < 0.000001f);

    // account for Y going down in cursor apis
    delta_cursor_y = -delta_cursor_y;

    // across = normalize(cross(normalize(look), normalize(up)))
    float across[3] = {
        look[1] / look_len * up[2] / up_len - look[2] / look_len * up[1] / up_len,
        look[2] / look_len * up[0] / up_len - look[0] / look_len * up[2] / up_len,
        look[0] / look_len * up[1] / up_len - look[1] / look_len * up[0] / up_len
    };
    float across_len = sqrtf(across[0] * across[0] + across[1] * across[1] + across[2] * across[2]);
    across[0] /= across_len;
    across[1] /= across_len;
    across[2] /= across_len;

    // forward = normalize(look)
    float forward[3] = { look[0] / look_len, look[1] / look_len, look[2] / look_len };

    // upward = normalize(cross(across, forward))
    float upward[3] = {
        across[1] * forward[2] - across[2] * forward[1],
        across[2] * forward[0] - across[0] * forward[2],
        across[0] * forward[1] - across[1] * forward[0]
    };
    float upward_len = sqrtf(upward[0] * upward[0] + upward[1] * upward[1] + upward[2] * upward[2]);
    upward[0] /= upward_len;
    upward[1] /= upward_len;
    upward[2] /= upward_len;

    // apply eye movement in the xz plane
    if ((right_held && !left_held) || (!right_held && left_held) ||
        (forward_held && !backward_held) || (!forward_held && backward_held))
    {
        float x_multiplier = (right_held ? 1.0f : 0.0f) - (left_held ? 1.0f : 0.0f);
        float z_multiplier = (forward_held ? 1.0f : 0.0f) - (backward_held ? 1.0f : 0.0f);

        float xz_movement[3] = {
            across[0] * x_multiplier + forward[0] * z_multiplier,
            across[1] * x_multiplier + forward[1] * z_multiplier,
            across[2] * x_multiplier + forward[2] * z_multiplier
        };

        float xz_movement_len = sqrtf(xz_movement[0] * xz_movement[0] + xz_movement[1] * xz_movement[1] + xz_movement[2] * xz_movement[2]);

        eye[0] += xz_movement[0] / xz_movement_len * eye_speed * delta_time_seconds;
        eye[1] += xz_movement[1] / xz_movement_len * eye_speed * delta_time_seconds;
        eye[2] += xz_movement[2] / xz_movement_len * eye_speed * delta_time_seconds;
    }

    // apply eye movement in the y direction
    if ((jump_held && !crouch_held) || (!jump_held && crouch_held))
    {
        float y_multiplier = (jump_held ? 1.0f : 0.0f) - (crouch_held ? 1.0f : 0.0f);
        float y_movement[3] = { up[0] * y_multiplier, up[1] * y_multiplier, up[2] * y_multiplier };
        float y_movement_len = sqrtf(y_movement[0] * y_movement[0] + y_movement[1] * y_movement[1] + y_movement[2] * y_movement[2]);

        eye[0] += y_movement[0] / y_movement_len * eye_speed * delta_time_seconds;
        eye[1] += y_movement[1] / y_movement_len * eye_speed * delta_time_seconds;
        eye[2] += y_movement[2] / y_movement_len * eye_speed * delta_time_seconds;
    }

    // apply yaw rotation (rotating left or right)
    if (delta_cursor_x != 0)
    {
        // rotation here is counter-clockwise because sin/cos are counter-clockwise
        float yaw_degrees = -delta_cursor_x * degrees_per_cursor_move;

        float yaw_radians = yaw_degrees * 3.14159265359f / 180.0f;
        float yaw_cos = cosf(yaw_radians);
        float yaw_sin = sinf(yaw_radians);

        float up_norm[3] = { up[0] / up_len, up[1] / up_len, up[2] / up_len };

        float yaw_rotation[9] = {
            yaw_cos + (1.0f - yaw_cos) * up_norm[0] * up_norm[0],
            (1.0f - yaw_cos)  * up_norm[0] * up_norm[1] + yaw_sin * up_norm[2],
            (1.0f - yaw_cos) * up_norm[0] * up_norm[2] - yaw_sin * up_norm[1],

            (1.0f - yaw_cos) * up_norm[0] * up_norm[1] - yaw_sin * up_norm[2],
            yaw_cos + (1.0f - yaw_cos) * up_norm[1] * up_norm[1],
            (1.0f - yaw_cos) * up_norm[1] * up_norm[2] + yaw_sin * up_norm[0],

            (1.0f - yaw_cos) * up_norm[0] * up_norm[2] + yaw_sin * up_norm[1],
            (1.0f - yaw_cos) * up_norm[1] * up_norm[2] - yaw_sin * up_norm[0],
            yaw_cos + (1.0f - yaw_cos) * up_norm[2] * up_norm[2]
        };

        float newlook[3] = {
            yaw_rotation[0] * look[0] + yaw_rotation[3] * look[1] + yaw_rotation[6] * look[2],
            yaw_rotation[1] * look[0] + yaw_rotation[4] * look[1] + yaw_rotation[7] * look[2],
            yaw_rotation[2] * look[0] + yaw_rotation[5] * look[1] + yaw_rotation[8] * look[2]
        };

        float newlook_len = sqrtf(newlook[0] * newlook[0] + newlook[1] * newlook[1] + newlook[2] * newlook[2]);

        look[0] = newlook[0] / newlook_len;
        look[1] = newlook[1] / newlook_len;
        look[2] = newlook[2] / newlook_len;
    }

    // apply pitch rotation (rotating up or down)
    if (delta_cursor_y != 0)
    {
        float rads_to_up = acosf(look[0] * up[0] + look[1] * up[1] + look[2] * up[2]);
        float rads_to_down = acosf(look[0] * -up[0] + look[1] * -up[1] + look[2] * -up[2]);

        float degs_to_up = rads_to_up / 3.14159265359f * 180.0f;
        float degs_to_down = rads_to_down / 3.14159265359f * 180.0f;

        float max_pitch_degrees = degs_to_up - (90.0f - max_pitch_rotation_degrees);
        if (max_pitch_degrees < 0.0f)
            max_pitch_degrees = 0.0f;

        float min_pitch_degrees = degs_to_down - (90.0f - max_pitch_rotation_degrees);
        if (min_pitch_degrees < 0.0f)
            min_pitch_degrees = 0.0f;

        // rotation here is counter-clockwise because sin/cos are counter-clockwise
        float pitch_degrees = delta_cursor_y * degrees_per_cursor_move;

        if (pitch_degrees > 0.0f && pitch_degrees > max_pitch_degrees)
            pitch_degrees = max_pitch_degrees;

        if (pitch_degrees < 0.0f && -pitch_degrees > min_pitch_degrees)
            pitch_degrees = -min_pitch_degrees;

        float pitch_rads = pitch_degrees * 3.14159265359f / 180.0f;
        float pitch_cos = cosf(pitch_rads);
        float pitch_sin = sinf(pitch_rads);

        float pitch_rotation[9] = {
            pitch_cos + (1.0f - pitch_cos) * across[0] * across[0],
            (1.0f - pitch_cos)  * across[0] * across[1] + pitch_sin * across[2],
            (1.0f - pitch_cos) * across[0] * across[2] - pitch_sin * across[1],

            (1.0f - pitch_cos) * across[0] * across[1] - pitch_sin * across[2],
            pitch_cos + (1.0f - pitch_cos) * across[1] * across[1],
            (1.0f - pitch_cos) * across[1] * across[2] + pitch_sin * across[0],

            (1.0f - pitch_cos) * across[0] * across[2] + pitch_sin * across[1],
            (1.0f - pitch_cos) * across[1] * across[2] - pitch_sin * across[0],
            pitch_cos + (1.0f - pitch_cos) * across[2] * across[2]
        };

        float newlook[3] = {
            pitch_rotation[0] * look[0] + pitch_rotation[3] * look[1] + pitch_rotation[6] * look[2],
            pitch_rotation[1] * look[0] + pitch_rotation[4] * look[1] + pitch_rotation[7] * look[2],
            pitch_rotation[2] * look[0] + pitch_rotation[5] * look[1] + pitch_rotation[8] * look[2]
        };

        float newlook_len = sqrtf(newlook[0] * newlook[0] + newlook[1] * newlook[1] + newlook[2] * newlook[2]);

        look[0] = newlook[0] / newlook_len;
        look[1] = newlook[1] / newlook_len;
        look[2] = newlook[2] / newlook_len;
    }

    mat4x4_look_at( (vec4*)view, eye, look, up );
}


#if 0
    enum CAMERA_MODE {
        CAMERA_FPS,
        CAMERA_ORTHO,
        CAMERA_DOF6, // FLY
        CAMERA_ARCBALL,
    };
    enum CAMERA_COORD {
        
    };
    lerp(cam1, cam2);
    present(lerp(cam1.render,cam2.render)); // crossfade
    camera_manager->setactive(cam2);
#endif


#if 0
    enum CAMERA_MODE {
        CAMERA_PERSPECTIVE,
        CAMERA_ORTHOGRAPHIC,
    };
    enum CAMERA_DISTANCE {
        CAMERA_REVERSEZ,
    };
    enum CAMERA_ORIGIN {
        CAMERA_TOPLEFT = 0x1,
        CAMERA_BOTTOMLEFT = 0x2,
        CAMERA_CENTERED = 0x4,
    };
    enum CAMERA_RANGE {
        CAMERA_UNIT = 0x10,
        CAMERA_RESOLUTION = 0x20,
    };

    // get camera projection matrix
    static inline
    float* cameraproj(float proj[16], int flags) {
        mat4x4 proj;
        if( flags & CAMERA_ORTHOGRAPHIC ) {
            /**/ if( flags & (CAMERA_TOPLEFT|CAMERA_UNIT))          mat4x4_ortho(proj, -1, +1, -1, +1, -1, 1 );       // 0,0 top-left, 1-1 bottom-right
            else if( flags & (CAMERA_TOPLEFT|CAMERA_RESOLUTION))    mat4x4_ortho(proj, -w/2, w/2, h/2, -h/2, -1, 1 ); // 0,0 top-left, w-h bottom-right
            else if( flags & (CAMERA_BOTTOMLEFT|CAMERA_RESOLUTION)) mat4x4_ortho(proj, 0, w, 0, h, -1, 1 );           // 0,0 bottom-left, w-h top-right
        }
        return proj;
    }
#endif

#endif

/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/


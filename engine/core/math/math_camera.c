// assumes look_delta is already multiplied by sensitivity (like, +0.02)
// assumes move_delta is already multiplied by frame delta time and acceleration factor.
// position is vec3(right, up, forward) convention. rotation is (xdelta, ydelta).

API void flycamera(
    mat44 cam_view,
    vec3 *position, vec2 *rotation,
    vec3 move_delta, vec2 look_delta );


#ifdef CAMERA_C
#pragma once

#include "math_linear.c"

void flycamera(
    mat44 cam_view,
    vec3 *position, vec2 *rotation,
    vec3 move_delta, vec2 look_delta )
{
    // apply look delta to mouse rotation
    rotation->x -= look_delta.y;
    rotation->y -= look_delta.x;

    // build rotation
    float rotationY[16], rotationX[16], rotationYX[16];
    rotation44(rotationX, rotation->x, 1.0f, 0.0f, 0.0f);
    rotation44(rotationY, rotation->y, 0.0f, 1.0f, 0.0f);
    multiply44(rotationYX, rotationY, rotationX);

    // apply move delta to head position
    vec3 worldMovement = transform44(rotationYX, move_delta);
    *position = add3(*position, worldMovement);

    // construct view matrix
    float inverseRotation[16], inverseTranslation[16];
    transpose44(inverseRotation, rotationYX);
    translation44(inverseTranslation, -position->x, -position->y, -position->z);
    multiply44(cam_view, inverseRotation, inverseTranslation); // = inverse(translation(position) * rotationYX);
}

#include <math.h>
#include <assert.h>

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
        mat4 proj;
        if( flags & CAMERA_ORTHOGRAPHIC ) {
            /**/ if( flags & (CAMERA_TOPLEFT|CAMERA_UNIT))          mat4_ortho(proj, -1, +1, -1, +1, -1, 1 );       // 0,0 top-left, 1-1 bottom-right
            else if( flags & (CAMERA_TOPLEFT|CAMERA_RESOLUTION))    mat4_ortho(proj, -w/2, w/2, h/2, -h/2, -1, 1 ); // 0,0 top-left, w-h bottom-right
            else if( flags & (CAMERA_BOTTOMLEFT|CAMERA_RESOLUTION)) mat4_ortho(proj, 0, w, 0, h, -1, 1 );           // 0,0 bottom-left, w-h top-right
        }
        return proj;
    }
#endif

#endif

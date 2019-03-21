#ifndef CAMERA_H
#define CAMERA_H

#include <stdbool.h>
#include "math_linear.c"

typedef struct camera {
    vec3 position;
    vec3 right, up, forward;

    float last_x, last_y, yaw, pitch;
    float sensitivity, invert_x, invert_y;
    float enabled;

    mat44 view;
} camera;

API void camera_create(camera *cam, float sensitivity, bool invert_x, bool invert_y);
API void camera_enable(camera *cam, bool enabled);

API void camera_fps(camera *cam, vec3 keyboard, vec2 mouse);
API void camera_orbit(camera *cam, float distance, vec2 mouse);

#endif


#ifdef CAMERA_C
#pragma once

void camera_create(camera *cam, float sensitivity, bool invert_x, bool invert_y) {
    cam->position = vec3(0, 0, -3);

    cam->right = vec3(1, 0, 0);
    cam->up = vec3(0, 1, 0);
    cam->forward = vec3(0, 0, 1);

    cam->last_x = 0;
    cam->last_y = 0;
    cam->yaw = 0;
    cam->pitch = 0;

    cam->sensitivity = sensitivity;
    cam->invert_x = invert_x ? -1 : 1;
    cam->invert_y = invert_y ? -1 : 1;
    cam->enabled = 1;

    identity44(cam->view);
}

void camera_enable(camera *cam, bool enabled) {
    cam->enabled = enabled;
}

void camera_fps(camera* cam, vec3 keyboard, vec2 mouse) {
    // look: update angles
    float offset_x = ( mouse.x - cam->last_x ) * cam->sensitivity * cam->invert_x;
    float offset_y = ( mouse.y - cam->last_y ) * cam->sensitivity * cam->invert_y;

    cam->last_x = mouse.x;
    cam->last_y = mouse.y;

    if( cam->enabled ) {
        cam->yaw += offset_x;
        cam->pitch += offset_y;

        // look: limit pitch angle [-89..89]
        cam->pitch = cam->pitch > 89 ? 89 : cam->pitch < -89 ? -89 : cam->pitch;

        // look: update forward vector
        cam->forward = norm3( vec3(
            cos(rad(cam->yaw)) * cos(rad(cam->pitch)),
            sin(rad(cam->pitch)), 
            sin(rad(cam->yaw)) * cos(rad(cam->pitch))
        ) );

        // look: recompute right vector
        cam->right = norm3(cross3(cam->forward, cam->up));

        // move: offset position
        float right = keyboard.x, top = keyboard.y, front = keyboard.z;
        cam->position = add3(cam->position, mul3(cam->forward, vec3(front, front, front)));
        cam->position = add3(cam->position, mul3(cam->up, vec3(top, top, top)));
        cam->position = add3(cam->position, mul3(cam->right, vec3(right, right, right)));
    }

    // compute view matrix
    lookat44(cam->view, cam->position, add3(cam->position, cam->forward), cam->up);
}

void camera_orbit( camera *cam, float distance, vec2 mouse ) {
    // look: update angles
    float offset_x = ( mouse.x - cam->last_x ) * cam->sensitivity * cam->invert_x;
    float offset_y = ( mouse.y - cam->last_y ) * cam->sensitivity * cam->invert_y;

    cam->last_x = mouse.x;
    cam->last_y = mouse.y;

    if( cam->enabled ) {
        cam->yaw += offset_x;
        cam->pitch += offset_y;

        // look: limit pitch angle [-89..89]
        cam->pitch = cam->pitch > 89 ? 89 : cam->pitch < -89 ? -89 : cam->pitch;
    }

    // compute view matrix
    float x = rad(cam->yaw), y = rad(cam->pitch), cx = cosf(x), cy = cosf(y), sx = sinf(x), sy = sinf(y);
    lookat44(cam->view, vec3( cx*cy*distance, sy*distance, sx*cy*distance ), vec3(0,0,0), vec3(0,1,0) );
}

// old api

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

// ideas

void camera_lerp();

#endif

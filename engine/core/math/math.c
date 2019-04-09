#ifdef CONFIG_H
#include CONFIG_H
#endif

#ifdef MATH_C
#pragma once
#define LINEAR_C
#define NOISE_C
#define TRANSFORM_C
#define UTILS_C
#define CAMERA_C
#define GJK_C
#define MTYPES_C
#endif

#include "math_linear.c"
#include "math_noise.c"
#include "math_transform.c"
#include "math_utils.c"
#include "math_camera.c"
#include "math_gjk.c"
#include "math_types.c"

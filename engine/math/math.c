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
#define GEOMETRY_C
#define EASE_C
#define SHAPE_C
#endif

#include "math_linear.c"
#include "math_noise.c"
#include "math_transform.c"
#include "math_utils.c"
#include "math_camera.c"
#include "math_gjk.c"
#include "math_types.c"
#include "math_geometry.c"
#include "math_ease.c"
#include "math_shape.c"

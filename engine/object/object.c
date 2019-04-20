#ifdef CONFIG_H
#include CONFIG_H
#endif

#ifdef OBJECT_C
#pragma once
#define ECS_C
#define REFLECT_C
#define JSON5_C
#endif

#include "object_reflect.c"
#include "object_ecs.c"
#include "object_json5.c"

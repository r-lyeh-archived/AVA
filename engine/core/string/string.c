#ifdef CONFIG_H
#include CONFIG_H
#endif

#ifdef STRING_C
#pragma once
#define QUARK_C
#define TYPE_C
#define SSO_C
#endif

#ifndef API
#define API
#endif

#include "string_quark.c"
#include "string_type.c"
#include "string_sso.c"

#ifdef CONFIG_H
#include CONFIG_H
#endif

#ifdef STRING_C
#pragma once
#define STRING2_C
#define UTILITIES_C

#define FUZZY_C
#define INI_C
#define QUARK_C
#define REGEX_C
#define SSO_C
#define UNICODE_C
#endif

#include "string2.c"
#include "string_utilities.c"

#include "string_fuzzy.c"
#include "string_ini.c"
#include "string_quark.c"
#include "string_regex.c"
#include "string_sso.c"
#include "string_unicode.c"

#ifdef CONFIG_H
#include CONFIG_H
#endif

#ifdef STRING_C
#pragma once
#define FORMAT_C
#define FUZZY_C
#define INI_C
#define NORMALIZE_C
#define QUARK_C
#define REGEX_C
#define SPLIT_C
#define SSO_C
#define TRIM_C
#define UNICODE_C
#define STRING2_C
#define STRING3_C
#endif

#include "string_quark.c"
#include "string_sso.c"
#include "string_ini.c"
#include "string_trim.c"
#include "string_regex.c"
#include "string_format.c"
#include "string_split.c"
#include "string_unicode.c"
#include "string_fuzzy.c"
#include "string_normalize.c"

#include "string2.c"
#include "string3.c"

/*
#include "string_sprintf.c"
#include "string_eval.c"
#include "string_transform.c"
#include "string_replace.c"
#include "string_option.c"
#include "string_intern.c"
#include "string_utf.c"
#include "string_hash.c"
*/

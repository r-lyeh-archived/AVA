#ifndef PLATFORM_H
#define PLATFORM_H

// detect language

#define C99 0
#define CPP 0

#ifdef __cplusplus
#undef  CPP
#define CPP 1
#else
#undef  C99
#define C99 1
#endif

// detect compiler

#define MSC 0
#define GCC 0
#define CLANG 0

#  if defined _MSC_VER
#undef  MSC
#define MSC 1
#elif defined __clang__ // before gcc
#undef  CLANG
#define CLANG 1
#elif defined __GNUC__
#undef  GCC
#define GCC 1
#endif

// detect platform

#define AND 0
#define BSD 0
#define IOS 0
#define LIN 0
#define OSX 0
#define PS4 0
#define RPI 0
#define STK 0
#define UNX 0
#define WEB 0
#define WIN 0
#define XB1 0

#  if defined __EMSCRIPTEN__
#undef  WEB
#define WEB           1
#elif defined _XBOX_ONE || defined _DURANGO
#undef  XB1
#define XB1           1 // before windows
#elif defined _WIN32 || defined __MINGW32__
#undef  WIN
#define WIN           1
#elif defined __ANDROID__
#undef  AND
#define AND           1 // before linux
#elif defined __ORBIS__
#undef  PS4
#define PS4           1 // before linux
#elif defined __STEAMLINK__
#undef  STK
#define STK           1 // before linux
#elif defined __VCCOREVER__
#undef  RPI
#define RPI           1 // before linux
#elif defined __linux__
#undef  LIN
#define LIN           1 // before unix
#elif defined __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ || defined __ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__
#undef  IOS
#define IOS           1 // before bsd
#elif defined __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ || defined __APPLE__
#undef  OSX
#define OSX           1 // before bsd
#elif defined __NetBSD__ || defined __FreeBSD__ || defined __OpenBSD__ || defined __DragonFly__
#undef  BSD
#define BSD           1 // before unix
#elif defined __unix__
#undef  UNX
#define UNX           1
#endif

// conditional build IF/IFN macros

#define IF(x, true_, ...)  IF_(x, true_, __VA_ARGS__)
#define IFN(x, true_, ...) IF_(x, __VA_ARGS__, true_)
#define IF_(x, true_, ...) IF_##x(true_, __VA_ARGS__)
#define IF_0(true_,...)    __VA_ARGS__
#define IF_1(true_,...)    true_

#endif

#if DETECT_DEMO
#include <stdio.h>
int main() {
    const char *lang = IF(C99, "C", "C++");
    IF(WIN, printf("win+%s", lang), printf("non-win+%s", lang));
}
#endif

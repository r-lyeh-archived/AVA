#ifndef PLATFORM_H
#define PLATFORM_H

// build type
#define DEBUG          0
#define DEBUGOPT       0
#define RELEASE        0

// endianness symbols
#define BIG            0
#define LITTLE         0

// arch wordsize and cacheline symbols
#define BITS32         0
#define BITS64         0
#define CACHE64        0
#define CACHE128       0

// architecture symbols
#define ARM            0
#define PPC            0
#define INTEL          0
#define JIT            0

// language symbols
#define C89            0
#define C99            0
#define C11            0
#define CPP            0
#define CPP03          0
#define CPP11          0
#define EXCEPTIONS     0

// compiler symbols
#define CLANG          0
#define COMPAT         0
#define GCC            0
#define ICC            0
#define MINGW          0
#define MSC            0
#define SNC            0

// platform symbols
#define ANDROID        0
#define BSD            0
#define HTML5          0
#define IOS            0
#define LINUX          0
#define OSX            0
#define PS4            0
#define RASPBERRYPI    0
#define STEAMLINK      0
#define UNIX           0
#define WINDOWS        0
#define XBOX1          0

// detect build type
#if defined _DEBUG && defined __OPTIMIZE__
#   undef  DEBUGOPT
#   define DEBUGOPT    1
#elif defined _DEBUG
#   undef  DEBUG
#   define DEBUG       1
#else
#   undef  RELEASE
#   define RELEASE     1
#endif

// detect compiler
// also compiler arm __CC_ARM, ibm __xlc__ || __xlC__, sun __SUNPRO_C || __SUNPRO_CC
#if   defined __ICC // __ICL, __INTEL_COMPILER
#   undef  ICC
#   define ICC         1
#elif defined __SNC__
#   undef  SNC
#   define SNC         1
#elif defined __clang__
#   undef  CLANG
#   define CLANG       1
#elif defined __GNUC__
#   undef  GCC
#   define GCC         1
#elif defined _MSC_VER
#   undef  MSC
#   define MSC         1
#elif defined __MINGW32__
#   undef  MINGW
#   define MINGW       1
#endif

// detect compilers in gcc compatibility mode (icc, snc)
#if defined __GNUC__ || defined __clang__
#   undef  COMPAT
#   define COMPAT      1
#endif

// detect C++ language and version
#ifdef __cplusplus
#   undef  CPP
#   define CPP         1
#   if (__cplusplus < 201103L && !defined _MSC_VER) || (defined _MSC_VER && _MSC_VER < 1700)
#   undef  CPP03
#   define CPP03       1
#   else
#   undef  CPP11
#   define CPP11       1
#   endif
#else // detect C language and version
#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L
#   undef  C11
#   define C11         1
#elif defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#   undef  C99
#   define C99         1
#else
#   undef  C89
#   define C89         1
#endif
#endif

// detect c++ exceptions
#if defined __cplusplus && ( \
    (defined _HAS_EXCEPTIONS && _HAS_EXCEPTIONS) || \
    (defined _STLP_USE_EXCEPTIONS && _STLP_USE_EXCEPTIONS) || \
    (defined HAVE_EXCEPTIONS) || \
    (defined __EXCEPTIONS) || \
    (defined _CPPUNWIND) )
#   undef  EXCEPTIONS
#   define EXCEPTIONS  1
#endif

#ifdef __has_feature
#if __has_feature(cxx_exceptions)
#   undef  EXCEPTIONS
#   define EXCEPTIONS  1
#endif
#endif

// detect endianness
// also BE if (*(uint16_t *)"\0\xff" < 0x100)
#if defined __BIG_ENDIAN__ || defined __ARMEB__ || defined __THUMBEB__ || defined __MIPSEB__ || (defined __BYTE_ORDER__ && __BYTE_ORDER__==__BIG_ENDIAN)
#   undef  BIG
#   define BIG         1
#else
#   undef  LITTLE
#   define LITTLE      1
#endif

// detect architecture
// also __mips__ __mips (L64), __ia64,__powerpc, __sparc, __alpha, __hppa, __avr32__, __sh__
#if   defined _M_ARM
#   undef  ARM
#   define ARM         1
#elif defined _M_PPC
#   undef  PPC
#   define PPC         1
#elif defined _M_IX86 || defined _M_X64
#   undef  INTEL
#   define INTEL       1
#else // likely VM/JIT
#   undef  JIT
#   define JIT         1
#endif

// detect wordsize
#include <stdint.h>
#  if defined INTPTR_MAX && INTPTR_MAX == INT32_MAX
#   undef  BITS32
#   define BITS32      1
#elif defined INTPTR_MAX && INTPTR_MAX == INT64_MAX
#   undef  BITS64
#   define BITS64      1
#endif

// detect cache line size
#if PPC
#   undef  CACHE128
#   define CACHE128    1
#else
#   undef  CACHE64
#   define CACHE64     1
#endif

// detect platform
// also CYG __CYGWIN__, QNX __QNXNTO__, SOL __sun, __hpux, _AIX, __WINDOWS__ (watcom), _WINDOWS
#  if defined __EMSCRIPTEN__
#   undef  HTML5
#   define HTML5       1
#elif defined _XBOX_ONE || defined _DURANGO
#   undef  XBOX1
#   define XBOX1       1 // before windows
#elif defined _WIN32 || defined __MINGW32__
#   undef  WINDOWS
#   define WINDOWS     1
#elif defined __ANDROID__
#   undef  ANDROID
#   define ANDROID     1 // before __linux__
#elif defined __ORBIS__
#   undef  PS4
#   define PS4         1 // before __linux__
#elif defined __STEAMLINK__
#   undef  STEAMLINK
#   define STEAMLINK   1 // before __linux__
#elif defined __VCCOREVER__
#   undef  RASPBERRYPI
#   define RASPBERRYPI 1 // before __linux__
#elif defined __linux__
#   undef  LINUX
#   define LINUX       1 // before __unix__
#elif defined __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ || defined __ENVIRONMENT_TV_OS_VERSION_MIN_REQUIRED__
#   undef  IOS
#   define IOS         1 // before bsd
#elif defined __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ || defined __APPLE__
#   undef  OSX
#   define OSX         1 // before bsd
#elif defined __NetBSD__ || defined __FreeBSD__ || defined __OpenBSD__ || defined __DragonFly__
#   undef  BSD
#   define BSD         1 // before __unix__
#elif defined __unix__
#   undef  UNIX
#   define UNIX        1
#endif

// conditional build IFDEF/IFNDEF macros

#define IFDEF(x,T,...)       IF_(x)(T,__VA_ARGS__/*F*/)
#define IFNDEF(x,T,...)      IF_(x)(__VA_ARGS__/*F*/,T)
#define IF_(x)            IF_DETECT(x)
#define IF_DETECT(...)    IF_DETECT##__VA_ARGS__
#define IF_DETECT0(t,...) __VA_ARGS__
#define IF_DETECT1(t,...) t

#endif

#if DETECT_DEMO
#include <stdio.h>
int main() {
    // compile-time if
    int debug = 0; IFDEF(DEBUG, debug = 1);
    // compile-time if-else
    const char *arch = IFDEF(BITS64, "64", "32") "-bit arch";
    // compile-time if-else-if
    const char *cpu = "CPU-" IFDEF(INTEL, "Intel", IFDEF(ARM, "ARM", "Unknown"));

    // symbols are boolean preprocessor directives as well
    #if DEBUG
    debug = 1;
    #endif

    // debug all symbols
    puts( 1 + 
        IFDEF(BITS32, "+BITS32") IFDEF(BITS64, "+BITS64")
        IFDEF(CACHE64, "+CACHE64") IFDEF(CACHE128, "+CACHE128")

        IFDEF(BIG, "+BIG") IFDEF(LITTLE, "+LITTLE")
        IFDEF(ARM, "+ARM") IFDEF(PPC, "+PPC") IFDEF(INTEL, "+INTEL") IFDEF(JIT, "+JIT")

        IFDEF(DEBUG, "+DEBUG") IFDEF(DEBUGOPT, "+DEBUGOPT") IFDEF(RELEASE, "+RELEASE")

        IFDEF(C89, "+C89") IFDEF(C99, "+C99") IFDEF(C11, "+C11")
        IFDEF(CPP, "+CPP") IFDEF(CPP03, "+CPP03") IFDEF(CPP11, "+CPP11") IFDEF(EXCEPTIONS, "+EXCEPT")

        IFDEF(MSC, "+MSC") IFDEF(SNC, "+SNC") IFDEF(ICC, "+ICC") IFDEF(GCC, "+GCC")
        IFDEF(CLANG, "+CLANG") IFDEF(MINGW, "+MINGW") IFDEF(COMPAT, "+COMPAT")
        
        IFDEF(HTML5, "+HTML5") 
        IFDEF(BSD, "+BSD") IFDEF(UNIX, "+UNIX")
        IFDEF(IOS, "+IOS") IFDEF(ANDROID, "+ANDROID")
        IFDEF(XBOX1, "+XBOX1") IFDEF(PS4, "+PS4")
        IFDEF(WINDOWS, "+WINDOWS") IFDEF(LINUX, "+LINUX") IFDEF(OSX, "+OSX")
        IFDEF(RASPBERRYPI, "+RASPBERRYPI") IFDEF(STEAMLINK, "+STEAMLINK")
    );
}
#endif

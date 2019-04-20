// ensure assert() is present in all debug/release builds, unless SHIPPING is defined

#ifdef SHIPPING
#   include <assert.h>
#   undef assert
#   define assert(expr)
#elif defined NDEBUG
#   undef NDEBUG
#   include <assert.h>
#   define NDEBUG
#else
#   include <assert.h>
#endif


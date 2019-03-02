#ifndef API_H
#define API_H

// api macros

#  if !defined(API) && defined(__cplusplus)
#define API extern "C" LINKAGE
#elif !defined(API)
#define API            LINKAGE
#endif

#ifndef DLL
#define DLL(name, version, ...) __VA_ARGS__
#endif

#endif

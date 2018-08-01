#ifndef LINKAGE_H
#define LINKAGE_H

// symbol linkage and visibility

#if   !defined(IMPORT) && defined(_MSC_VER)
#define IMPORT __declspec(dllimport)
#elif !defined(IMPORT)
#define IMPORT __attribute__((visibility("default")))
#endif

#if   !defined(EXPORT) && defined(_MSC_VER)
#define EXPORT __declspec(dllexport)
#elif !defined(EXPORT)
#define EXPORT __attribute__((visibility("default")))
#endif

#ifndef STATIC
#define STATIC
#endif

#ifndef LINKAGE
#define LINKAGE STATIC
#endif

#endif

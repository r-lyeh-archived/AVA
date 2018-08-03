#ifndef LINKAGE_H
#define LINKAGE_H

// symbol linkage and visibility

#ifdef _MSC_VER
#define IMPORT __declspec(dllimport)
#define EXPORT __declspec(dllexport)
#define STATIC
#else
#define IMPORT extern
#define EXPORT __attribute__((visibility("default")))
#define STATIC
#endif

#ifndef LINKAGE
#define LINKAGE STATIC
#endif

/*
#if   !defined(IMPORT) && defined(_MSC_VER)
#define IMPORT __declspec(dllimport)
#elif !defined(IMPORT)
#define IMPORT extern
#endif

#if   !defined(EXPORT) && defined(_MSC_VER)
#define EXPORT __declspec(dllexport)
#elif !defined(EXPORT)
#define EXPORT __attribute__((visibility("default")))
#endif

#ifndef STATIC
#define STATIC
#endif
*/

#endif

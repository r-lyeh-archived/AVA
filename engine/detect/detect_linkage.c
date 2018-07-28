#ifndef LINKAGE_H
#define LINKAGE_H

// symbol linkage and visibility

#ifndef EXPORT
#define EXPORT __declspec(dllexport)
#endif

#ifndef IMPORT
#define IMPORT __declspec(dllimport)
#endif

#ifndef STATIC
#define STATIC
#endif

#ifndef LINKAGE
#define LINKAGE STATIC
#endif

#endif

// # inline

#pragma once

#if defined(__GNUC__)
#define __inline inline
#endif

#if defined(_MSC_VER) && !defined(__cplusplus) && !defined(__inline)
#define inline __inline
#endif

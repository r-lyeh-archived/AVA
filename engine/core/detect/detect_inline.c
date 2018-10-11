// # inline

#pragma once

#if defined(__GNUC__)
#define INLINE inline
#elif defined(_MSC_VER)
#define INLINE __inline
#endif

// # thread local #############################################################

#pragma once

#ifdef __GNUC__
#   define THREAD_LOCAL __thread
#elif defined _MSC_VER || defined __INTEL_COMPILER
#   define THREAD_LOCAL __declspec(thread)
#elif __STDC_VERSION__ >= 201112L
#   define THREAD_LOCAL _Thread_local
#else
#   define THREAD_LOCAL
#endif

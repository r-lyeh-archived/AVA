// # thread local #############################################################

#pragma once

#if defined _MSC_VER || defined __INTEL_COMPILER
#define __thread __declspec(thread)
#elif !defined(__GNUC__)
#define __thread
#endif

/*
#ifdef __GNUC__
#   define THREAD_LOCAL __thread
#elif __STDC_VERSION__ >= 201112L
#   define THREAD_LOCAL _Thread_local
#elif defined(_MSC_VER) | defined(__INTEL_COMPILER)
#   define THREAD_LOCAL __declspec(thread)
#else
#   error Cannot define THREAD_LOCAL
#endif
*/

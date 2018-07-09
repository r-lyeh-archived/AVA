#pragma once

#define builtin(x, ...) builtin_##x (__VA_ARGS__)

#ifdef _MSC_VER
#define builtin_thread() __declspec(thread)
#else
#define builtin_thread() __thread
#endif

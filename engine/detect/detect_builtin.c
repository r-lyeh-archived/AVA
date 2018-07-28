#ifndef BUILTIN_H
#define BUILTIN_H

// builtin macros

#define builtin(x, ...) builtin_##x (__VA_ARGS__)

#if MSC
#define builtin_inline() __inline
#define builtin_thread() __declspec(thread)
#define builtin_format_string()
#define builtin_format_vargs(num_args)
#define builtin_puts(text) puts(text)
#define builtin_callstack(num_traces) STRDUP("")
#define builtin_inline() __inline
#else
#define builtin_inline() inline
#define builtin_thread() __thread
#define builtin_format_string()
#define builtin_format_vargs(num_args)
#define builtin_puts(text) puts(text)
#define builtin_callstack(num_traces) STRDUP("")
#define builtin_inline() inline
#endif

#endif

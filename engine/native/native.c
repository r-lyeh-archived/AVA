#ifdef CONFIG_H
#include CONFIG_H
#endif

#ifdef NATIVE_C
#pragma once
#define DLL_C
#define EXEC_C
#define CONSOLE_C
#define ENV_C
#define TTY_C
#endif

#include "native_dll.c"
#include "native_exec.c"
#include "native_console.c"
#include "native_environment.c"
#include "native_tty.c"

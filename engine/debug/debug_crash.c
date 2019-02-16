void trap();
void crash();

#ifdef CRASH_C
#pragma once
#include <stdio.h>
#include <assert.h>
#include <signal.h>
#include "debug_panic.c"
#include "debug_logger.c"
#include "debug_build.c"

// ---
//
#if _MSC_VER && defined __cplusplus
#   include <winsock2.h>
#   include <eh.h> // _set_se_translator
static void _cdecl se_translator( unsigned int ex, EXCEPTION_POINTERS *p ) {
    switch( ex ) {
        default: //panic( "Unknown exception" );
        break; case EXCEPTION_ACCESS_VIOLATION: //panic( "Memory access violation" );
        break; case EXCEPTION_ILLEGAL_INSTRUCTION: //panic( "Illegal instruction" );
        break; case EXCEPTION_INT_DIVIDE_BY_ZERO: //panic( "Integer divide by zero" );
        break; case EXCEPTION_STACK_OVERFLOW: //panic( "Stack overflow" );
        {}
    };
    panic("se_translator(): exception caught");
}
#endif
#include <signal.h>
static void catch(int signum) {
    signal(signum, catch); // reset for next signal
    panic("!catch found");
}
#ifndef _WIN32
#include <sys/resource.h> // setrlimit()
#endif
void trap() { // trapc(), trapcpp()
    // install signal handlers
    const int signals[] = { // The C standard defines following 6 signals:
        SIGABRT,      // abort: abnormal termination.
        SIGFPE,       // floating point exception.
        SIGILL,       // illegal: invalid instruction.
#if !SHIPPING
        SIGINT,       // interrupt: interactive attention request sent to the program.
#endif
        SIGSEGV,      // segmentation violation: invalid memory access.
        SIGTERM,      // terminate: termination request sent to the program.
    #ifdef __APPLE__
        SIGPIPE,
    #endif
    #ifdef _WIN32
        SIGBREAK,
    #else
        SIGBUS,       // Bus error
        SIGHUP,       // sent to a process when its controlling terminal is closed
        //SIGQUIT,    //
        //SIGKILL,    // kernel will let go of the process without informing the process of it
        //SIGTRAP,    // debugger
        //SIGSYS,
    #endif
    };
    for( int i = 0; i < sizeof(signals)/sizeof(signals[0]); ++i ) {
        signal(signals[i], SIG_IGN); //catch);
    }
#ifdef __cplusplus
    // C++11
    set_terminate(catch);
#if _MSC_VER
    _set_se_translator( se_translator );
#endif
#endif
#ifndef _WIN32
    // enable coredumps
    rlimit core_limit = { RLIM_INFINITY, RLIM_INFINITY };
    setrlimit( RLIMIT_CORE, &core_limit );
#endif
}


#ifdef _WIN32
#include <winsock2.h>
#include <dbghelp.h>
#include <sys/stat.h>
#include <stdlib.h>

int random() {
    return srand(time(0)), rand() % RAND_MAX;
}

void create_mini_dump(EXCEPTION_POINTERS *ex, const char *path) {
    typedef BOOL(*PDUMPFN) (
        HANDLE hProcess,
        DWORD ProcessId,
        HANDLE hFile,
        MINIDUMP_TYPE DumpType,
        PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
        PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
        PMINIDUMP_CALLBACK_INFORMATION CallbackParam
    );

    HANDLE hFile = CreateFileA(path, GENERIC_READ | GENERIC_WRITE,
        0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    HMODULE lib = LoadLibraryA("DbgHelp.dll");
    PDUMPFN pFn = (PDUMPFN)(uintptr_t)GetProcAddress(lib, "MiniDumpWriteDump");

    if( hFile && hFile != INVALID_HANDLE_VALUE ) {
        MINIDUMP_EXCEPTION_INFORMATION mdei;
        mdei.ThreadId = GetCurrentThreadId();
        mdei.ExceptionPointers = ex;
        mdei.ClientPointers = TRUE;

        BOOL rv = (*pFn)(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, (ex != 0) ? &mdei : 0, 0, 0);
        CloseHandle(hFile);
    }
}

LONG WINAPI crash_reporter(PEXCEPTION_POINTERS exc_info) {
    char path[1024];
    sprintf(path, "%s/build(%s)_date(%s)_crash(%d).dmp", "./", build_stamp(), current_stamp(), random() % 999 );
    create_mini_dump(exc_info, path);

    puts(build_shortid());

    {
        FILE* fp = fopen(path, "rb");
        if (!fp) {
            return EXCEPTION_EXECUTE_HANDLER;
        }

        struct stat file_info;
        if (fstat(fileno(fp), &file_info)) {
            return EXCEPTION_EXECUTE_HANDLER;
        }

        // upload(fp, "https://server:port/crash_dump");
        fclose(fp);
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

#if defined AUTORUN && (defined RELEASE && RELEASE)
AUTORUN {
    LOG(CRASH.C, "unhandled exception handler installed...");

    SetUnhandledExceptionFilter(crash_reporter);

    // tests:
    // int *p = 0; *p += 42;
    // throw 1;
}
#endif

#endif

void crash() {
    catch(42);
}

#endif

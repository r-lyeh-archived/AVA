// ## debug: backtraces
// Windows users add `/Zi` compilation flags, else add `-g` and/or `-ldl` flags
// If you are linking your binary using GNU ld you need to add --export-dynamic
// - rlyeh, public domain.

char *callstack( int traces );

#ifdef CALLSTACK_C
#pragma once
#include "detect_threadlocal.c"
#include "../memory/memory_realloc.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__APPLE__)
#include <execinfo.h> // backtrace, backtrace_symbols
#include <dlfcn.h>    // dladdr, Dl_info
#elif defined(__GNUC__)
#include <execinfo.h>  // backtrace, backtrace_symbols
#elif defined(_WIN32)
#include <winsock2.h>  // windows.h alternative
#include <dbghelp.h>
#pragma comment(lib, "DbgHelp.lib")
#pragma comment(lib, "Kernel32.lib")
static int backtrace( void **addr, int maxtraces ) { //$
    static HANDLE process = 0;
    if( !process ) process = GetCurrentProcess();
    if( !process ) exit( puts( "error: no current process" ) );
    static int init = 0;
    if( !init ) init = SymSetOptions(SYMOPT_UNDNAME), SymInitialize( process, NULL, TRUE );
    if( !init ) exit( puts( "error: cannot initialize DbgHelp.lib" ) );

    typedef USHORT (WINAPI *pFN)(__in ULONG, __in ULONG, __out PVOID*, __out_opt PULONG);
    static pFN rtlCaptureStackBackTrace = 0;
    if( !rtlCaptureStackBackTrace ) {
        rtlCaptureStackBackTrace = (pFN)GetProcAddress(LoadLibraryA("kernel32.dll"), "RtlCaptureStackBackTrace");
    }
    if( !rtlCaptureStackBackTrace ) {
        return 0;
    }
    return rtlCaptureStackBackTrace(1, maxtraces, (PVOID *)addr, (DWORD *) 0);
}
static char **backtrace_symbols(void *const *array,int size) { //$
    HANDLE process = GetCurrentProcess();
    enum { MAXSYMBOLNAME = 512 - sizeof(IMAGEHLP_SYMBOL64) };
    char symbol64_buf     [ 512 ];
    char symbol64_bufblank[ 512 ] = {0};
    IMAGEHLP_SYMBOL64 *symbol64       = (IMAGEHLP_SYMBOL64*)symbol64_buf;
    IMAGEHLP_SYMBOL64 *symbol64_blank = (IMAGEHLP_SYMBOL64*)symbol64_bufblank;
    symbol64_blank->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
    symbol64_blank->MaxNameLength = (MAXSYMBOLNAME-1) / 2; //wchar?

    int symlen = size * sizeof(char *);
    char **symbols = (char **)MALLOC(symlen);

    if( symbols ) {
        for( int i = 0; i < size; ++i ) {
            symbols[ i ] = NULL;
        }

        char begin[1024];
        for( int i = 0; i < size; ++i ) {
            char **symbuf, *buffer = begin;

            DWORD64 dw1 = 0, dw2 = 0;
            *symbol64 = *symbol64_blank;
            if( SymGetSymFromAddr64( process, (DWORD64)array[i], &dw1, symbol64 ) ) {
                IMAGEHLP_LINE64 L64 = {0};
                L64.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
                if( SymGetLineFromAddr64( process, (DWORD64)array[i], (DWORD*)&dw2, &L64 ) ) {
                    // int lenbase( const char *str );
                    int base = 0; // lenbase( L64.FileName );
                    buffer += sprintf(buffer,"%s (%s:%d)%c", symbol64->Name, &L64.FileName[ base ], L64.LineNumber, 0);
                } else {
                    buffer += sprintf(buffer,"%s (??)%c", symbol64->Name, 0);
                }
            } else  buffer += sprintf(buffer,"(??)%c", 0);

            size_t buflen = buffer - begin + 1;

            symbuf = (char **)REALLOC( symbols, symlen + buflen );
            if( symbuf ) {
                memcpy( (char *)symbuf + symlen, begin, buflen );
                symbuf[ i ] = (char *)(size_t)symlen;

                symbols = symbuf;
                symlen += buflen;
            } else break;
        }

        for( int i = 0; i < size; ++i ) {
            symbols[ i ] = (char *)symbols + (size_t)symbols[ i ];
        }
    }

    return symbols;
}
#else
static int backtrace(void **, int) { return 0; }
static char **backtrace_symbols(void *const *,int) { return 0; }
#endif

void callstack_cb( int traces, int (*yield)(const char *)) { //$
    enum { skip = 1 }; /* exclude 1 trace from stack (this function) */
    enum { maxtraces = 128 };

    int inc = 1;
    if( traces < 0 ) traces = -traces, inc = -1;
    if( traces == 0 ) return;
    if( traces > maxtraces ) traces = maxtraces;

    void *stack[ maxtraces ];
    traces = backtrace( stack, traces );
    char **symbols = backtrace_symbols( stack, traces );

    char demangled[1024], buf[1024];
    int L = 0, B = inc>0 ? skip - 1 : traces, E = inc>0 ? traces : skip - 1;
    for( int i = B; ( i += inc ) != E; ) {
#ifdef __linux__
        char *address = strstr( symbols[i], "[" ) + 1; address[strlen(address) - 1] = '\0';
        char *binary = symbols[i]; strstr( symbols[i], "(" )[0] = '\0';
        char command[1024]; sprintf(command, "addr2line -e %s %s", binary, address);
        FILE *fp = popen( command, "rb" );
        if( !fp ) {
            exit( puts( "cannot invoke 'addr2line'" ) );
        }
        char *line_p = fgets(demangled, sizeof(demangled), fp);
        symbols[i] = demangled;
        pclose(fp);
#elif __APPLE__
        struct Dl_info info;
        if( dladdr(stack[i], &info) && info.dli_sname ) {
            char *dmgbuf = info.dli_sname[0] != '_' ? NULL :
                 __cxa_demangle(info.dli_sname, NULL, 0, NULL);
            strcpy( demangled, dmgbuf ? dmgbuf : info.dli_sname );
            symbols[i] = demangled;
            FREE( dmgbuf );
        }
#endif
        //sprintf(buf, "%3d %#016p %s", ++L, stack[i], symbols[i]);
        sprintf(buf, "%3d %s", ++L, symbols[i]);
        if( yield(buf) < 0 ) break;
    }

    FREE( symbols );
}

static THREAD_LOCAL char *callstack_strbuf[128] = {0};
static THREAD_LOCAL int callstack_counter = 0, callstack_len = 0;
int callstack_(const char *line) {
    int len = strlen(line);
    callstack_len += len + 1;
    callstack_strbuf[callstack_counter] = (char*)REALLOC(callstack_strbuf[callstack_counter], (len * 1.5));
    strcpy(callstack_strbuf[callstack_counter], line );
    callstack_counter = (callstack_counter +1) % 128;
    return 1;
}
HEAP
char *callstack( int traces ) { //$
    //if( callstack_ ) callstack_str_ = REALLOC(callstack_str_, callstack_lenbuf_ = 0);
    callstack_counter = callstack_len = 0;
    callstack_cb( traces, callstack_ );
    char *buf = (char*)MALLOC( callstack_len + 1 ); buf[0] = 0; 
    for( int i = 0; i < callstack_counter; ++i ) {
        strcat(buf, callstack_strbuf[i] ); // <-- optimize
        strcat(buf, "\n");
    }
    return buf;
}
#endif

#ifdef CALLSTACK_DEMO
#include <stdio.h>
void B() { 
    callstack_cb(-64, puts);
    puts("--");
    callstack_cb(+64, puts);

    char *cs = callstack(+16);
    puts( cs ? cs : "" );
    free( cs );
}
void A() {
    B();
}
int main() {
    A();
}
#endif

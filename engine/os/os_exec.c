#ifndef EXEC_H
#define EXEC_H

/*HEAP*/ char *os_exec( const char *cmd );
bool os_task( const char *cmd, int *progress );

#endif

#ifdef EXEC_C
#pragma once

/*
popen... pclose
creates async task
captures log
scan each line
if (%d %, %03d %, %d%, %03d%) found, update progress of task
at end, close task and call callback

os_exec( cmd, args, []{ prog }, []{ err }, []{ ok } );
*/

#include "../detect/detect.c"
#include "../core/core_realloc.c"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if MSC
#define popen  _popen
#define pclose _pclose
#endif

//HEAP
char *os_exec( const char *cmd ) {
    // char *buf = (char *)MALLOC( 4096 );
    static char buf[4096]; memset(buf, 0, 4096);
    FILE *fp = popen( cmd, "rt" );
    if( fp ) {
        while( fgets(buf, 4096 - 1, fp) ) {
            char *r = strrchr(buf, '\r');
            char *n = strrchr(buf, '\n');
            if( r ) *r = 0;
            if( n ) *n = 0;
            //puts(buf);
        }
    }
    pclose(fp);
    return buf;
}

bool os_task( const char *cmd, int *progress ) {
    int ok = 1;
    char buf[4096] = {0};
    FILE *fp = popen( cmd, "rt" ); // also check 'unbuffer {cmd}' under linux
    if( fp ) {
        while( fgets(buf, 4096 - 1, fp) ) {
            char *r = strrchr(buf, '\r');
            char *n = strrchr(buf, '\n');
            if( r ) *r = 0;
            if( n ) *n = 0;
            //
            for( int i = 0; buf[i]; ++i ) if( buf[i] >= 'A' && buf[i] <= 'Z' ) buf[i] = buf[i] - 'A' + 'a';
            if( strstr(buf, "error") || strstr(buf, "fail") ) ok = 0;
            if( progress ) {
                char *peek = strchr(buf, '%');
                if( peek ) {
                    int estimate_len = peek - buf;
                    if( estimate_len >= 4 ) {
                        /**/ if( peek[-4] >= '0' && peek[-4] <= '9' ) *progress = atoi(&peek[-4]);
                        else if( peek[-3] >= '0' && peek[-3] <= '9' ) *progress = atoi(&peek[-3]);
                        else if( peek[-2] >= '0' && peek[-2] <= '9' ) *progress = atoi(&peek[-2]);
                        else if( peek[-1] >= '0' && peek[-1] <= '9' ) *progress = atoi(&peek[-1]);
                    }
                }
            }
        }
    }
    pclose(fp);
    return ok;
}

#endif

#if 0
// worker.c
#include <stdio.h>
#include <windows.h>
int main() {
    setvbuf(stdout,NULL,_IOLBF,2);
    for( int j = 0; j <= 100; ++j ) {
        if( j == 50 ) printf("runtime-error! cant do sth\n");
        printf("worker %03d%%\n", j);
        Sleep(100);
    }
}
#endif

#if 0 && defined __cplusplus
#include <thread>
#include <stdio.h>
int main() {
    int progress = 0;
    std::thread( [&] {
        do {
            printf("%d%% ...", progress);
            std::this_thread::sleep_for( std::chrono::seconds(1) );
        } while(progress < 100);
    } ).detach();
    printf("result = %d\n", ptask("worker", &progress));
}
#elif 0
#include <stdio.h>
int main() {
    puts( os_exec("worker") );
}
#endif

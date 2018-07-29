// simple udp/tcp client/server code
// - rlyeh, public domain.

#ifndef SOCKET_H
#define SOCKET_H

enum {
    SOCKET_BIND = 1,
    SOCKET_BLOCK = 2,
    SOCKET_DELAY = 4,
    // SOCKET_RELIABLE = 64,
};

API int  socket_init();
API void socket_drop(void);

API int  socket_connect( const char *url, int flags );
API int  socket_send( int s, const void *buf, int len );
API int  socket_recv( int s, void *buf, int len );
API int  socket_close( int s );

API int  socket_accept( int s );

#endif

#ifdef SOCKET_C
#pragma once
#include <string.h>
#include "../core/core_autorun.c"

#define SWRAP_IMPLEMENTATION
#include "3rd/swrap.h"
#pragma comment(lib, "ws2_32.lib")

#define socket_die(x) exit(printf("ERROR L%d %s : " x " (errno: %s)\n", __LINE__, __FUNCTION__, strerror(errno) ))

static int socket_isudp( int s ) {
    return s > 0;
}
static int socket_handle( int s ) {
    return ( s > 0 ? s : -s ) - 1;
}

int socket_init() {
    int ec = swrapInit();
    if( ec ) socket_die("swrapInit error");
    return 1;
}
void socket_drop(void) {
    swrapTerminate();
}

int socket_connect(const char *url, int flags) {
    char *uri = strstr(url, "://" ); uri = uri ? uri + 3 : url;
    char *port = strchr(uri, ':'); port = port ? port + 1 : "4000";
    char addr[1024] = {0}; sscanf( uri, "%[^:\0]", addr );

    int pr = !memcmp(url, "udp://", 6) ? SWRAP_UDP : SWRAP_TCP;
    int mo = flags & SOCKET_BIND ? SWRAP_BIND : SWRAP_CONNECT;
    int bl = flags & SOCKET_BLOCK ? 0 : SWRAP_NOBLOCK;
    int dl = flags & SOCKET_DELAY ? 0 : SWRAP_NODELAY;

    int s = swrapSocket(pr, mo, bl | dl, addr, port);
    if( s == -1 ) socket_die("swrapSocket error");

    if( (flags & SOCKET_BIND) && (pr == SWRAP_TCP) ) {
        int ec = swrapListen(s, 10000); /* maximum backlog */
        if(ec != 0) socket_die("swrapListen error");
    }

    return (s + 1) * (pr == SWRAP_TCP ? -1 : +1);
}

// @todo: return number of bytes sent?
int socket_send( int s, const void *buf, int len ) {
    int ec = swrapSend(socket_handle(s), (char *)buf, len);
    if( ec == -1 ) return 0; //socket_die("swrapSend error");
    return 1;
}

int socket_accept( int s ) {
    if( socket_isudp(s) ) return s;
    struct swrap_addr sa = {0};
    int ec = swrapAccept(socket_handle(s), &sa);
    if( ec < 0 ) return 0; /* no new connections */
    s = (ec + 1) * -1;
    return s;
}

// return number of received bytes, 0 if no bytes were received (error, shutdown or timeout)
int socket_recv( int s, void *buf, int len ) {
    int ec = swrapSelect(socket_handle(s), 0.00001);
    if( ec < 0 ) return -1; //socket_die("swrapSelect error");
    if( ec == 0 ) return 0; /* timeout */
    //> 0: new data is availabale

    // returns the number of bytes received, 0 on orderly shutdown, or -1 on failure (e.g. no data to receive)
    struct swrap_addr sa = {0};
    ec = swrapReceiveFrom(socket_handle(s), &sa, buf, len);
    if( ec < 0 ) return -1; // socket_die("swrapReceiveFrom error");
    if( ec == 0 ) { // orderly shutdown
        return 0; // returned as timeout
    }

#if 0
    char host[128], serv[128];
    int ec2 = swrapAddressInfo(&sa, host, 128, serv, 128 );
    if( ec2 != 0 ) die("swrapAddressInfo error");
    printf("server: recv %d bytes from %s:%s : %.*s\n", ec, host, serv, ec, buf );
#endif

    return ec;
}

int socket_close( int s ) {
    if( !socket_isudp(s) ) swrapClose(socket_handle(s));
    return 1;
}

AUTORUN {
    if( socket_init() ) {
        atexit(socket_drop);
    }
}

#endif

#ifdef SOCKET_DEMO
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>

#ifdef _MSC_VER
#include <omp.h>
#define clock_t        double
#define clock          omp_get_wtime
#define CLOCKS_PER_SEC 1.0f
#else
#include <time.h>
#endif

//at most INT_MAX bytes of data will be received, buffer sizes greater than INT_MAX have no additional benefit
#define MAX_PACKET 32768
#define MTU_PACKET 508

#ifndef N
#define N 1000 //0000
#endif

int packets = 0;
uint64_t bytes = 0;

void server_stats(int dummy) {
    printf("server: %d pkt, %d bytes/pkt, %lld recv/bytes\n", packets, MAX_PACKET, bytes );
    exit(0);
}

void server( const char *url ) {
    puts("server listening");

    signal(SIGINT, server_stats);

    int s = socket_connect( url, SOCKET_BIND | SOCKET_BLOCK );

    int blen;
    char *buf = malloc(blen = MAX_PACKET);

    for( ;; ) {
        int s2 = socket_accept(s);
        if( s2 ) {
            int ec;
            while( (ec = socket_recv(s2, buf, blen)) >= 0 ) {
                packets ++;
                bytes += ec;
            }
            socket_close(s2);
        }
    }
}

void client( const char *url ) {
    int blen;
    char* buf = malloc(blen = MAX_PACKET);
    puts("hello from client");

    clock_t then = clock();

    int s = socket_connect(url, SOCKET_BLOCK);
    for( int i = 0; i < N; ++i ) {
        if (socket_send(s, buf, blen)) {
            packets++;
            bytes += blen;
        }
    }
    socket_close(s);

    double t = (clock() - then) / (double)CLOCKS_PER_SEC;

    printf("client: %d pkt, %d bytes/pkt, %lld recv/bytes, ", packets, MAX_PACKET, bytes );
    printf("%5.2fs = %5.2fM pkt/s, %5.2f pkt/frame, ", t, (packets/t) / 1000000, (packets * 0.01667) / t );
    printf("%5.2f MiB/s\n", (bytes/t) / 1024 / 1024 );
}

int main(int argc, char **argv) {
    char *mode = argc > 1 ? argv[1] : "client";
    char *url = argc > 2 ? argv[2] : "127.0.0.1:4000";
    printf("%s (%s)\n", url, mode);

    if( mode[0] == 's' /*"server"*/ ) {
        server( url );
    } else {
        client( url );
    }
}

#endif


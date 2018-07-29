// Network message event dispatcher,
// - rlyeh, public domain.
// 
// Take the time to read the OSC spec, http://opensoundcontrol.org/spec-1_0
// Then, some differences:
// - message: basic types subset (C-format style: %c %i %u %f %s ...) 
// - bundles = chained\nmessages\n
// - pattern-matching dispatch rules (bash/batch style: wildcards ?*.. in message paths)
// 
// @todo: add timestamp to bundles
// @todo: select short,int,varint depending on lossless conversion
// @todo: select half,float,double depending on lossless conversion
// @todo: escape strings
// @todo: blobs

#ifndef DISPATCH_H
#define DISPATCH_H

API void dispatch_new( const char *route, void (*callback)(const char *message) );
API void dispatch_bundle( char *bundle );
API void dispatch_message( const char *route, const char *message );

API char *bundle_new( char *bundle, const char *addr, const char *msg);

#endif


#ifdef DISPATCH_C
#pragma once
#include "../core/core_realloc.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct dispatcher {
    const char *route;
    void(*callback)( const char *message );
} dispatcher;

static dispatcher *table = 0; // table[128] = {0};
static int num = 0;

void dispatch_new( const char *route, void (*callback)(const char *message) ) {
    table = REALLOC(table, (++num) * sizeof(dispatcher) );
    table[num - 1].route = route;
    table[num - 1].callback = callback;
}

char *bundle_new( char *bundle, const char *addr, const char *msg) {
    char *p = REALLOC( bundle, (bundle ? strlen(bundle) : 0) + strlen(addr) + 1 + strlen(msg) + 1 );
    sprintf( p + ( bundle ? strlen(p) - 1 : 0), "%s\n%s\n", addr, msg );
    return p;
}

void dispatch_bundle( char *bundle ) {
    char addr[256], msg[256], *p;
    while( *bundle ) {
        for( p = addr; *bundle != '\n'; ) *p++ = *bundle++; bundle++; *p++ = 0;
        for( p = msg; *bundle != '\n'; ) *p++ = *bundle++; bundle++; *p++ = 0;
        dispatch_message( addr, msg );
    }
}

static int strmatch( const char *text, const char *pattern ) {
    if( *pattern=='\0' ) return !*text;
    if( *pattern=='*' )  return strmatch(text, pattern+1) || (*text && strmatch(text+1, pattern));
    if( *pattern=='?' )  return *text && (*text != '.') && strmatch(text+1, pattern+1);
    return (*text == *pattern) && strmatch(text+1, pattern+1);
}

void dispatch_message( const char *route, const char *message ) {
    for( int i = 0; i < num; ++i ) {
        if( strmatch(table[i].route, route)) {
            table[i].callback( message );
        }
    }
}

#endif

#ifdef DISPATCH_DEMO
#include <stdio.h>

void freq1( const char *msg ) {
    float f; 
    sscanf(msg, "%f", &f);
    printf("freq: %f\n", f );
}
void shoot2( const char *msg ) {
    int x, y;
    sscanf(msg, "%d %d", &x, &y);
    printf("shoot-x: %d\n", x );
    printf("shoot-y: %d\n", y );
}

int main() {
    // server

    dispatch_new( "/oscillator/3/frequency", freq1 );
    dispatch_new( "/oscillator/4/frequency", freq1 );
    dispatch_new( "/game/shoot", shoot2 );

    // simulate incoming messages

    dispatch_message( "/oscillator/?/*", "440" );
    dispatch_message( "/game/s*t", "10 20" );

    // client: make bundle

    char *p = bundle_new(0, "/oscillator/?/*", "440" );
    p = bundle_new(p, "/game/s*t", "10 20" );
    puts(p); // verify bundle

    // server: simulate incoming bundle

    dispatch_bundle(p);
}
#endif

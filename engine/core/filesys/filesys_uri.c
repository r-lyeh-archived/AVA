// portable uri, relative, absolute path handling
// - rlyeh, public domain.

// assumes:
//                  /----------path---------\
//        /--addr--\/-----base----\/--file--\
// spec://host:port/folder1/folder2/name.type

// example:
// http://192.168.1.1:1234/dev/home/name.ext1.ext2
//
// spec: http://
// addr: 192.168.1.1:1234
// host: 192.168.1.1
// port: 1234
// path: /dev/home/name.ext1.ext2
// base: /dev/home/
// file: name.ext1.ext2
// name: file
// type: .ext1.ext2
// 

#ifndef URI_H
#define URI_H

// api

#ifndef URI_MAXPATH
#define URI_MAXPATH 4096
#endif

char *uri_spec( char output[URI_MAXPATH], const char *uri );
char *uri_addr( char output[URI_MAXPATH], const char *uri );
char *uri_host( char output[URI_MAXPATH], const char *uri );
char *uri_port( char output[URI_MAXPATH], const char *uri );
char *uri_full( char output[URI_MAXPATH], const char *uri );
char *uri_path( char output[URI_MAXPATH], const char *uri );
char *uri_file( char output[URI_MAXPATH], const char *uri );
char *uri_name( char output[URI_MAXPATH], const char *uri );
char *uri_type( char output[URI_MAXPATH], const char *uri );

// query

  int uri_isdir( const char *uri );
  int uri_isbin( const char *uri );
  int uri_isabs( const char *uri );
  int uri_isrel( const char *uri );
  int uri_isnorm( const char *uri );

// util

char *uri_norm( char output[URI_MAXPATH], const char *uri );

#endif

// ----------------------------------------------------------------------------

#ifdef URI_C
#pragma once
//#include "engine.h"
#include <string.h>
#include <stdio.h>

#if defined(_MSC_VER) && !defined(snprintf)
#define snprintf _snprintf
#endif

char *uri_spec( char output[URI_MAXPATH], const char *uri ) {
    const char *found = strstr( uri, "://" );
    snprintf( output, URI_MAXPATH, "%.*s", (int)(found ? found - uri + 3 : 0), uri );
    return output;
}
char *uri_addr( char output[URI_MAXPATH], const char *uri ) {
    const char *addr = &uri[strlen(uri_spec(output, uri))];
    const char *colon = strchr( addr, ':'), *slash = strchr( addr, '/');
    if( colon ) {
        snprintf( output, URI_MAXPATH, "%.*s", (int)(slash ? slash - addr : strlen(addr)), addr );
    } else {
        output[ 0 ] = 0;
    }
    return output;
}
char *uri_host( char output[URI_MAXPATH], const char *uri ) {
    const char *addr = &uri[strlen(uri_spec(output, uri))];
    const char *colon = strchr( addr, ':' ), *slash = strchr( addr, '/' );
    if( colon ) {
        colon = colon && slash ? ( colon < slash ? colon : slash ) : (colon < slash ? slash : colon);
        snprintf( output, URI_MAXPATH, "%.*s", (int)(colon ? colon - addr : 0), colon ? addr : "" );
    } else {
        output[0] = 0;
    }
    return output;
}
char *uri_port( char output[URI_MAXPATH], const char *uri ) {
    const char *addr = &uri[strlen(uri_spec(output, uri))];
    const char *colon = strchr( addr, ':' ), *colon2 = colon ? colon+1 : "";
    const char *slash = strchr( addr, '/' );
    snprintf( output, URI_MAXPATH, "%.*s", (int)(slash && colon ? slash-1 - colon : strlen(colon2)), colon2 );
    return output;
}
char *uri_full( char output[URI_MAXPATH], const char *uri ) {
    const char *addr = &uri[strlen(uri_spec(output, uri))];
    const char *colon = strchr( addr, ':'), *slash = strchr( addr, '/' );
    if( colon ) {
        snprintf( output, URI_MAXPATH, "%.*s", (int)(slash ? strlen(slash) : strlen(addr)), slash ? slash : addr );
        if( strchr(output, ':') ) output[0] = 0;
    } else {
        snprintf( output, URI_MAXPATH, "%s", addr );
    }
    return output;
}
char *uri_path( char output[URI_MAXPATH], const char *uri ) {
    char *s = strrchr( uri_full(output, uri), '/' );
    return s ? (s[1] = '\0', output) : (output[0] = '\0', output);
}
char *uri_file( char output[URI_MAXPATH], const char *uri ) {
    char *s = strrchr( uri_full(output, uri), '/' );
    const char *alt = !s && strrchr(uri, ':') ? "" : uri;
    return strcpy( output, s ? s+1 : alt );
}
char *uri_name( char output[URI_MAXPATH], const char *uri ) {
    char *t = strchr( uri_file( output, uri ), '.' );
    return t ? (t[0] = 0, output) : output;
}
char *uri_type( char output[URI_MAXPATH], const char *uri ) {
    char *s = strchr( uri_file( output, uri ), '.' );
    return strcpy( output, s ? s : "" );
}

char *uri_norm( char output[URI_MAXPATH], const char *uri ) {
    int i = 0;
    for( ; uri[i] ; ++i ) {
        output[i] = uri[i] == '\\' ? '/' : uri[i];
    }
    // !: add extra NUL in case user wants to add ending slash quickly.
    // ie, uri_norm(output, uri); output[ strlen(output) ] = '/';
    return output[i] = 0, output[i+1] = 0 /*!*/, output;
}


int uri_isdir( const char *uri ) {
    return uri && uri[0] && uri[ strlen(uri) - 1 ] == '/';
}

int uri_isbin( const char *uri ) {
    return uri && !uri_isdir(uri);
}

int uri_isabs( const char *uri ) {
    return uri && uri[0] && (uri[0] == '/' || uri[1] == ':');
}

int uri_isrel( const char *uri ) {
    return uri && !uri_isabs( uri );
}

int uri_isnorm( const char *uri ) {
    return uri && strchr(uri, '\\') == 0;
}

#endif

#ifdef URI_DEMO
#include <assert.h>
#include <stdio.h>
int main() {
    // sample
    char buf[URI_MAXPATH];
    printf( "%s\n", "http://192.168.1.1:1234/dev/home/name.ext1.ext2" );
    printf( "\tnorm: %s\n", uri_norm(buf, "http://192.168.1.1:1234\\dev\\home\\name.ext1.ext2") );
    
    printf( "\tspec: %s\n", uri_spec(buf, "http://192.168.1.1:1234/dev/home/name.ext1.ext2") );
    printf( "\taddr: %s\n", uri_addr(buf, "http://192.168.1.1:1234/dev/home/name.ext1.ext2") );
    printf( "\thost: %s\n", uri_host(buf, "http://192.168.1.1:1234/dev/home/name.ext1.ext2") );
    printf( "\tport: %s\n", uri_port(buf, "http://192.168.1.1:1234/dev/home/name.ext1.ext2") );

    printf( "\tpath: %s\n", uri_full(buf, "http://192.168.1.1:1234/dev/home/name.ext1.ext2") );
    printf( "\tbase: %s\n", uri_path(buf, "http://192.168.1.1:1234/dev/home/name.ext1.ext2") );
    printf( "\tfile: %s\n", uri_file(buf, "http://192.168.1.1:1234/dev/home/name.ext1.ext2") );
    printf( "\tname: %s\n", uri_name(buf, "http://192.168.1.1:1234/dev/home/name.ext1.ext2") );
    printf( "\ttype: %s\n", uri_type(buf, "http://192.168.1.1:1234/dev/home/name.ext1.ext2") );

    // tests
    #define spec "spec://"
    #define host "host"
    #define port "port"
    #define addr host ":" port
    #define name "name"
    #define type ".ext1.ext2"
    #define file name type
    #define base "/folder1/folder2/"
    #define path base file
    #define full spec addr path
    #define testURL(expr, spec, addr,host,port, path ) \
            assert( 0 == strcmp(uri_spec(buf, ""expr), ""spec ) ); \
            assert( 0 == strcmp(uri_addr(buf, ""expr), ""addr ) ); \
            assert( 0 == strcmp(uri_host(buf, ""expr), ""host ) ); \
            assert( 0 == strcmp(uri_port(buf, ""expr), ""port ) ); \
            assert( 0 == strcmp(uri_full(buf, ""expr), ""path ) );
    #define testPATH(expr, path, base,file, name,type) \
            assert( 0 == strcmp(uri_full(buf, ""expr), ""path ) ); \
            assert( 0 == strcmp(uri_path(buf, ""expr), ""base ) ); \
            assert( 0 == strcmp(uri_file(buf, ""expr), ""file ) ); \
            assert( 0 == strcmp(uri_name(buf, ""expr), ""name ) ); \
            assert( 0 == strcmp(uri_type(buf, ""expr), ""type ) );

    #define testALL(expr, spec, addr,host,port, path, base,file, name,type) \
            testURL(expr,spec,addr,host,port,path); \
            testPATH(expr,path, base,file, name,type);

    if( "test many spec and/or addr and/or path combinations" ) {
        testALL(               ,     ,     ,    ,    ,          ,    ,    ,    ,     );
        testALL(           path,     ,     ,    ,    , path     ,base,file,name,type );
        testALL(      addr     ,     , addr,host,port,          ,    ,    ,    ,     );
        testALL(      addr path,     , addr,host,port, path     ,base,file,name,type );
        testALL( spec          , spec,     ,    ,    ,          ,    ,    ,    ,     );
        testALL( spec      path, spec,     ,    ,    , path     ,base,file,name,type );
        testALL( spec addr     , spec, addr,host,port,          ,    ,    ,    ,     );
        testALL( spec addr path, spec, addr,host,port, path     ,base,file,name,type );
        testALL(           type,     ,     ,    ,    ,      type,    ,type,    ,type );
        testALL(      name     ,     ,     ,    ,    ,      name,    ,name,name,     );
        testALL(      name type,     ,     ,    ,    , name type,    ,file,name,type );
        testALL( base          ,     ,     ,    ,    , base     ,base,    ,    ,     );
        testALL( base      type,     ,     ,    ,    , base      type,base,type,    ,type );
        testALL( base name     ,     ,     ,    ,    , base name     ,base,name,name,     );
        testALL( base name type,     ,     ,    ,    , base name type,base,file,name,type );
   }
   if( "test more readable examples as well" ) {
        testURL( "http://192.168.1.1:1234/dev/home/file", "http://", "192.168.1.1:1234", "192.168.1.1", "1234", "/dev/home/file" );
        testURL( "file://lala:3201", "file://", "lala:3201", "lala", "3201", "");
        testURL( "file:///dev/home", "file://", "", "", "", "/dev/home");
        testURL( "tcp://server:3204/resource", "tcp://", "server:3204", "server", "3204", "/resource");
        testURL( "tcp://server:3202", "tcp://", "server:3202", "server", "3202", "");
        testURL( "tcp://server:/", "tcp://", "server:", "server", "", "/");
        testURL( "tcp://file", "tcp://", "", "", "", "file");
        testURL( "lala:1234/lala2", "", "lala:1234", "lala", "1234", "/lala2" );
        testURL( "lala:1234", "", "lala:1234", "lala", "1234", "" );
        testURL( "/lala/lala2", "", "", "", "", "/lala/lala2" );
        testURL( "lala", "", "", "", "", "lala" );
        testURL( "../~.home/lala/lala2", "", "", "", "", "../~.home/lala/lala2" );
        testPATH( "../~.home/lala/lala2..ext2", "../~.home/lala/lala2..ext2", "../~.home/lala/", "lala2..ext2", "lala2", "..ext2" );
    }

    if( "random tests from another project" ) {
        assert( 0 == strcmp( uri_norm(buf, "c:\\hello.txt"), "c:/hello.txt") );
        assert( 0 == strcmp( uri_norm(buf, "/A\\b//c"), "/A/b//c") );
        assert( 0 == strcmp( uri_full(buf, "/ab/c.e.xt"), "/ab/c.e.xt") );
        assert( 0 == strcmp( uri_path(buf, "/ab/c.e.xt"), "/ab/") );
        assert( 0 == strcmp( uri_file(buf, "/ab/c.e.xt"), "c.e.xt") );
        assert( 0 == strcmp( uri_name(buf, "/ab/c.e.xt"), "c") );
        assert( 0 == strcmp( uri_type(buf, "/ab/c.e.xt"), ".e.xt") );
        assert( 0 == strcmp( uri_full(buf, "c.e.xt"), "c.e.xt") );
        assert( 0 == strcmp( uri_path(buf, "c.e.xt"), "") );
        assert( 0 == strcmp( uri_file(buf, "c.e.xt"), "c.e.xt") );
        assert( 0 == strcmp( uri_name(buf, "c.e.xt"), "c") );
        assert( 0 == strcmp( uri_type(buf, "c.e.xt"), ".e.xt") );
    }

    if( "sanity checks" ) {
        assert(!uri_isnorm("c:\\hello.txt"));
        assert(!uri_isdir(0));
        assert(!uri_isbin(0));
        assert(!uri_isabs(0));
        assert(!uri_isrel(0));
        assert( uri_isdir("~user/"));
        assert( uri_isdir("/C/prj/"));
        assert( uri_isdir("./data/"));
        assert( uri_isbin("~user/abc.txt"));
        assert( uri_isbin("/C/prj/abc.txt"));
        assert( uri_isbin("./data/abc.txt"));
        assert( uri_isrel("~user/abc.txt"));
        assert( uri_isabs("/C/prj/abc.txt"));
        assert( uri_isabs("C:/prj/abc.txt"));
        assert( uri_isrel("./data/abc.txt"));
    }

    assert(~puts("Ok"));
}
#endif

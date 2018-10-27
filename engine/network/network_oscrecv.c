// simple osc server. designed for easy integration with a gameloop / immediate mode style.
// - rlyeh, public domain. forked from original code by @mmalex (public domain).
// 
// @todo
// - add support for // wildcard
// - add support for [ array parameters ] 
// - add OSC message generation & output

#ifndef OSCRECV_H
#define OSCRECV_H

// - create a listening socket.
// - call every frame. read the udp port and parse all messages found there.
// - count number of received messages, also return pointer to first item.
// - find most recent message matching 'addr'.

int osc_listen( const char *mask, const char *port );
int osc_update(int s); 
int osc_count(const struct osc_msg **first);

const struct osc_msg *osc_find(const char *addr);

// OSC types from the spec.

enum {
    OSC_INT = 'i',
    OSC_FLOAT = 'f',
    OSC_STRING = 's',
    OSC_SYMBOL = 'S',
    OSC_BLOB = 'b',
    OSC_INT64 = 'h',
    OSC_TIME = 't',
    OSC_DOUBLE = 'd',
    OSC_CHAR = 'c',
    OSC_RGBA = 'r',
    OSC_MIDI = 'm',
    OSC_TRUE = 'T',
    OSC_FALSE = 'F',
    OSC_NIL = 'N',
    OSC_INFINITY = 'I',
    // OSC_ARRAY = '[',  // @todo
};

// OSC message

#include <stdint.h>

typedef struct osc_msg {
    const char *pattern;// address in osc message
    const char *types;  // string of characters taken from the OSC types enum
    const char *data;   // pointer to raw osc data
    int64_t i[4];       // integer interpretation of first 4 params (for blobs & strings, is length)
    const char *s[4];   // for blobs and strings
    float f[4];         // floating point interpretation of first 4 params
} osc_msg;

#endif

#ifdef OSCRECV_C
#pragma once
#include <assert.h>
#include <string.h>
#include <math.h> // INFINITY

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h> // sockets
#include <ws2tcpip.h> // getaddrinfo
#pragma comment(lib,"ws2_32.lib")
#define sockaddr_in SOCKADDR_IN

#define OSC_MAX_BUF 65536
#define OSC_MAX_MESSAGES 1024

static char buf[OSC_MAX_BUF];
static struct osc_msg msg[OSC_MAX_MESSAGES];
static int bufpos;
static int msgpos;

int osc__parse(osc_msg *out, int maxmsg, const char *s, const char *e);

int osc__match(const char *pat, const char *addr) {
    for (int n=0;*pat;addr++,pat++) switch (*pat) {
    default:  if (*pat!=*addr) return 0; break;
    case '?': break;
    case '[': n=(*++pat=='!'); for (pat+=n; *pat!=']' && *pat;++pat) { if (pat[1]=='-') { if (*addr>=*pat && *addr<=pat[2]) { n=!n; break; } pat+=pat[2] ? 2 : 1; } else if (*pat==*addr) { n=!n; break; } }
              if (!n) return 0; while (*pat && *pat!=']') pat++; break;
    case '{': n=0; for (const char *p=++pat; *p && *p!='}' && *p!='/'; pat=++p) { while (*p && *p!='}' && *p!='/' && *p!=',') p++; if (!strncmp(pat,addr,p-pat)) { addr+=p-pat; n=1; break; } }
              while (*pat && *pat!='}') pat++; if (!n) return 0; break;
    case '*': while (pat[1]=='*') pat++; n=0; if (pat[1]=='/' || pat[1]==0) { while (*addr && *addr!='/') ++addr; n=1; } else for (;*addr!='/' && *addr; ++addr) if (osc__match(pat + 1, addr)) { n=1; break; }
              if (!n) return 0; addr--; break;
    // @todo: add // wildcard support
    }
    return *addr== 0;
}

int osc_listen( const char *mask, const char *port ) {
    static WSADATA wsadata = {0}, *init = 0; if( !init ) { init = &wsadata;
    WSAStartup(MAKEWORD(2,2),&wsadata);
    }

    struct addrinfo *addr = 0;
    struct addrinfo hints = {0};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_ADDRCONFIG;
    
    if( getaddrinfo(mask, port, &hints, &addr) != 0 ) {
        // failed to resolve remote socket address
        return 0;
    }
    int fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if( fd < 0 ) {
        // "failed to open socket %s", strerror(errno)
        return 0;
    }

    if( bind(fd,addr->ai_addr,addr->ai_addrlen) == -1 ) {
        // "failed to open socket %s", strerror(errno)
        return 0;
    }

    u_long nonblocking_enabled = TRUE;
    ioctlsocket( fd, FIONBIO, &nonblocking_enabled );

    freeaddrinfo(addr);
    return fd;
}

int osc_update(int fd) {
    if (fd<0) return 0;
    for (msgpos=0,bufpos=0;msgpos<OSC_MAX_MESSAGES && bufpos<OSC_MAX_BUF-8;) {
        sockaddr_in addr;
        int addrlen = sizeof(addr);
        int n=recvfrom(fd,buf+bufpos,OSC_MAX_BUF-bufpos-1,0,(struct sockaddr*)&addr,&addrlen); 
        if (n<=0)  // TODO - look at what the error was
            return 0;
        char *s=buf+bufpos; 
        s[n]=0; // null terminate packet always, for easier c handling
        msgpos+=osc__parse(msg+msgpos,OSC_MAX_MESSAGES-msgpos,s,s+n);    
        bufpos+=n+1;                
    }   
    return 1;
}

int osc_count(const osc_msg **first) {
    *first = msg;
    return msgpos;
}

const osc_msg *osc_find(const char *addr) { // search in reverse order, so newest wins
    for (int i=msgpos;i-->0;) if (osc__match(msg[i].pattern, addr)) return &msg[i];
    return 0;
}


float  osc__asfloat(uint32_t x)  { union { float f;  uint32_t i; } u; u.i=x; return u.f; }
double osc__asdouble(uint64_t x) { union { double f; uint64_t i; } u; u.i=x; return u.f; }

int osc__parse_i32(const char **s, const char *e) {
    if (*s+4>e) { *s=e; return 0; }
    int rv=htonl(*(uint32_t*)*s);
    *s+=4;
    return rv;
}
int64_t osc__parse_i64(const char **s, const char *e) {
    if (*s+8>e) { *s=e; return 0; }
    int rv=htonl(*(uint64_t*)*s);
    *s+=8;
    return rv;
}
const char *osc__parse_str(const char **s, const char *e) {
    int len=strlen(*s);
    const char *rv=*s;
    *s=rv+((len+4)&~3); 
    if (*s>e) *s=e;
    return rv;
}
const char *osc__parse_bin(const char **s, const char *e, int *len) {
    *len=osc__parse_i32(s,e); 
    int maxlen=e-*s;
    if (*len>maxlen) *len=maxlen;
    if (*len<0) *len=0;
    const char *rv=*s;
    *s=rv+((*len+3)&~3);
    if (*s>e) *s=e;
    return rv;
}
int osc__parse(osc_msg *out, int maxmsg, const char *s, const char *e) {
    if (maxmsg<=0 || s>=e) return 0;
    if (*(uint64_t*)s==*(uint64_t*)"#bundle\0") { // bundle is #bundle, uint64_t time, uint32_t length, <osc packet>        
        osc__parse_i64(&s,e); // skip time for now. TODO
        int msgcount=0;
        while (s<e && msgcount<maxmsg) {
            int len=osc__parse_i32(&s,e);
            int maxlen=e-s;
            if (len>maxlen) len=maxlen;
            if (len<0) len=0;
            int n=osc__parse(out+msgcount, maxmsg-msgcount, s, s+len);
            msgcount+=n;
            s+=((len+3)&~3);
        }
        return msgcount;
    }
    // single message
    memset(out,0,sizeof(osc_msg));
    out->pattern=osc__parse_str(&s,e);
    if (!out->pattern)
        return 0;
    out->types=(*s==',')?osc__parse_str(&s,e):",f";
    if (!out->types) 
        return 0;
    out->types++;
    out->data=s;
    for (int param=0;param<4;++param) {
        int f2i=0;
        switch (out->types[param]) {
            default: return 1; // done!
            case OSC_CHAR: case OSC_RGBA: // all int32...
            case OSC_MIDI: case OSC_INT:        out->i[param]=osc__parse_i32(&s,e); break;
            case OSC_TIME: case OSC_INT64:      out->i[param]=osc__parse_i64(&s,e); break;
            case OSC_STRING: case OSC_SYMBOL:   out->s[param]=osc__parse_str(&s,e); out->i[param]=strlen(out->s[param]); break;
            case OSC_FLOAT:                     out->f[param]=osc__asfloat(osc__parse_i32(&s,e)); f2i=1; break;
            case OSC_DOUBLE:                    out->f[param]=osc__asdouble(osc__parse_i32(&s,e)); f2i=1; break;
            case OSC_BLOB:                      {int len=0; out->s[param]=osc__parse_bin(&s,e,&len); out->i[param]=len; break; }
            case OSC_INFINITY:                  out->f[param]=INFINITY; out->i[param]=0x7fffffff; break;
            case OSC_TRUE:                      out->i[param]=1; break;
            case OSC_FALSE: case OSC_NIL:       out->i[param]=0; break;
//          case OSC_ARRAY:                     @todo
        }
        if (f2i)
            out->i[param]=(int64_t)(out->f[param]+0.5f);
        else
            out->f[param]=out->i[param];
    }
    return 1;
}


void osc_match_tests() {
    // @mmalex's tests
    assert(  osc__match("/[a-c]/?/[abc]/*/fish*/*food/f*/{foo,bar,baz}","/a/b/c//fishfood/monkeyfood/f/bar") );
    assert( !osc__match("/[a-c]/?/[abc]/*/fish*/*food/f*/{fog,bar,baz}","/a/b/c//fishfood/monkeyfood/f/foo") );
    assert( !osc__match("/[a-c]/?/[abc]/*/fith*/*food/f*/{foo,bar,baz}","/a/b/c//fishfood/monkeyfood/f/foo") );
    assert( !osc__match("/[a-c]/?/[abc]/*/fish*/*good/f*/{foo,bar,baz}","/a/b/c//fishfood/monkeyfood/f/foo") );
    assert( !osc__match("/[a-c]/?/[abc]/*/fish*/*food/g/{foo,bar,baz}","/a/b/c//fishfood/monkeyfood/f/foo") );
    assert(  osc__match("/[fa-cd]/?/[abc]/*/fish*/*food/f*/{foo,bar,baz}","/d/b/c//fishfood/monkeyfood/f/bar") );

    // Julien Pommier's oscpkt tests
    assert( !osc__match("//bar", "bar"));
//  assert(  osc__match("//bar", "/bar"));
//  assert(  osc__match("//bar", "/foo/plop/bar"));
//  assert(  osc__match("/foo//", "/foo/plop/df/"));
//  assert(  osc__match("/foo///////bar", "/foo/plop/baz/bar"));
    assert(  osc__match("*", "bar"));
    assert(  osc__match("/foo/*", "/foo/bar"));
//  assert(  osc__match("/{bar,fo}/b[aA]r", "/fo/bar"));
//  assert( !osc__match("/{bar,fo}/b[aA]r", "/foo/bar"));
//  assert(  osc__match("/fo{bar,}/ba[e-t]", "/fo/bar"));
    assert( !osc__match("/fo{bar,}/ba[t-z]", "/fo/bar"));
//  assert(  osc__match("/f{,ioio,bar}o/?a[!a]", "/fo/bar"));
    assert(  osc__match("/foo/bar", "/foo/bar"));
    assert(  osc__match("/f*o/bar", "/foo/bar"));
    assert(  osc__match("/fo*o/bar", "/foo/bar"));
//  assert(  osc__match("/*//bar", "/foo/bar"));
    assert(  osc__match("/*/bar", "/foo/bar"));
    assert(  osc__match("/*o/bar", "/foo/bar"));
    assert(  osc__match("/*/*/*/*a***/*/*/*/*/", "/foo/bar/foo/barrrr/foo/bar/foo/barrrr/"));
    assert( !osc__match("/*/*/*/**/*/*/*/*/q", "/foo/bar/foo/barrrr/foo/bar/foo/barrrr/p"));
    assert(  osc__match("[-]", "-"));
//  assert(  osc__match("[a-]", "a"));
//  assert(  osc__match("[a-]", "-"));
}

#endif


#ifdef OSCRECV_DEMO
#include <stdio.h>
int main() {
    osc_match_tests();

    int fd = osc_listen( "127.0.0.1", "9000" );
    if( fd ) for(;;) {
        static unsigned char counter = 0;
        printf("\r%c listening to 9000", "\\|/-"[ counter = (counter+1) & 3 ]);

        Sleep(100);
        osc_update(fd);

        const osc_msg *begin;
        for( int it = 0, end = osc_count(&begin); it < end; ++it ) {
            const osc_msg *msg = begin + it;
            printf("> %s\n", msg->pattern);
        }
    }
}

/*
// IMGUI OSC WRAPPER HELPER DEMO
// usage: ImGui::SliderFloat(...) with ImOsc::SliderFloat("/oscaddr/foo",...)
namespace ImOsc {
    const float *osc_findFloat(const char *addr) {
        // Small extension which is that if the address starts 0,1,2,3:
        //     "1/accxyz" as opposed to normal "/accxyz" for example
        // then 0,1,2,3 refers to the argument that the slider is bound to. thus you can read the Y coordinate
        // of XY pads, the Z coordinate of accelerometers, and so on. This syntax is not necessary in the Osc*
        // 'lower level' functions, its just a hack to let the ImOsc:: ImGui-ish interface to avoid an extra parameter.
        int chan=0; if (*addr>='0' && *addr<='3') chan=*addr++-'0';
        if (const osc_msg *msg=osc_find(addr)) return &msg->f[chan];
        return 0;
    }
    bool SliderFloat(const char *osc_addr, const char* label, float* v, float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) {
        if (const float *f=osc_findFloat(osc_addr)) *v=v_min+(v_max-v_min)*(*f);
        return ::ImGui::SliderFloat(label,v,v_min,v_max,display_format,power);
    }
};
*/

#endif

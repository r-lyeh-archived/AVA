/*
simple experimental WIP osc server designed for easy integration with a gameloop / immediate mode style.

I hereby place this in the public domain. do what you want with it, but no promises its fit for any
purpose! let me know if you use it, or find problems with it. cheers - Im @mmalex on twitter

the core of the API is just 2 functions:

void OscUpdate(int fd); // call every frame. read the udp port and parse all messages read from udp socket fd
const OscMessage *OscFind(const char *addr); // find most recent message matching 'addr' 

the idea is the call OscUpdate() every frame (ie regularly) and it reads and concats as many UDP packets 
as it can from the OS's socket buffer into a single static buffer. 
it then parses these into a float list of OscMessages which contain convenient copies of useful fields, 
such as timestamp, the first 4 params cast to float and int (regardless of original OSC type), and pointers 
back into the static buffer for strings like OSC address pattern.
You can trivially get a pointer to all these parsed messages via OscGetAll() and do what you want with it.

OscFind is a simple wrapper on OscGetAll that searches from newest to oldest, looking for a message that 
matches the given address. the idea is that you can scatter OscFind throughout your gamecode, as near as 
possible to where it should take affect. 

IMGUI example integration:

An example of this is the trivial ImGui::SliderFloat wrapper
at the bottom of this file, which just looks for a matching message, pokes the user's float, and calls 
on to ImGui as usual. In this way, you can just replace 

ImGui::SliderFloat(...) with ImOsc::SliderFloat("/oscaddr/foo",...)

...to make your slider OSC controllable (eg via TouchOsc on the iphone/ipad or any other OSC client)

The ImOsc::SliderFloat interface has a small extension which is that if the address starts 0,1,2,3:
    "1/accxyz" as opposed to normal "/accxyz" for example
...then 0,1,2,3 refers to the argument that the slider is bound to. thus you can read the Y coordinate
of XY pads, the Z coordinate of accelerometers, and so on. This syntax is not necessary in the Osc*
'lower level' functions, its just a hack to let the ImOsc:: ImGui-ish interface to avoid an extra parameter.

there's lots still to do, this is so far a 1 day hack.
- find bugs :)
- fix bugs :)
- add support for // wildcard
- add support for [ array parameters ] 
- add OSC message generation & output
- wrap the entire ImGui:: control set (floats, ints, sliders, graphs, buttons, checkboxes, etc)
- etc

whether any of those get done is... unknown ;)
enjoy!
@mmalex

ps building:
this has no 'dependencies' other than simple C posix/sockets, and ImGui if you want to try that wrapper obviously
Ive only tested this on windows so far.
OscUpdate() expects the file descriptor of a non-blocking, UDP socket bound to the port you want to listen on.
ie you need to call socket(), bind() and set non blocking on it, yourself.
*/

#ifndef OSCRECV_H
#define OSCRECV_H

#include <stdint.h>

typedef struct OscMessage OscMessage;
int OscMatch(const char *pattern, const char *addr); // match a pattern to a fixed address, per the OSC spec (ish).

int OscUpdate(int fd); // read the udp port and parse all messages found there
const OscMessage *OscFind(const char *addr); // find most recent message matching 'addr' 
const OscMessage *OscGetAll(const OscMessage **last_out); // return all messages parsed by OscUpdate

// 

enum { // OSC types from the spec. 
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
    OSC_INFINITUM = 'I',
    //TODO OSC_ARRAY = '[', 
};

struct OscMessage {
    const char *pattern;// address in osc message
    const char *types; // string of characters taken from the OSC types enum
    const char *data;  // pointer to raw osc data
    int64_t i[4];      // integer interpretation of first 4 params (for blobs & strings, is length)
    const char *s[4];   // for blobs and strings
    float f[4];    // floating point interpretation of first 4 params
};


/*  // IMGUI OSC WRAPPER HELPERS DEMO!
    namespace ImOsc {
        // syntax hack - leading [0123] digit selects paramter index 0-3
        const float *OscFindFloat(const char *addr) { int chan=0; if (*addr>='0' && *addr<='3') chan=*addr++-'0'; if (const OscMessage *msg=OscFind(addr)) return &msg->f[chan]; return 0; }
        
        bool SliderFloat(const char *osc_addr, const char* label, float* v, float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f) {
            if (const float *f=OscFindFloat(osc_addr)) *v=v_min+(v_max-v_min)*(*f);
            return ::ImGui::SliderFloat(label,v,v_min,v_max,display_format,power);
        }

        // TODO - etc -for all other types
    };
*/

#endif

#ifdef OSCRECV_C
#pragma once
#include <math.h> // for INFINITY
#include <string.h>
#include <assert.h>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <errno.h>
#include <stdio.h>
#include <winsock2.h> // for sockets
#include <ws2tcpip.h> // for getaddrinfo
#pragma comment(lib,"ws2_32.lib") // for sockets
#define sockaddr_in SOCKADDR_IN

#define OSC_MAX_BUF 65536
#define OSC_MAX_MESSAGES 1024

static char buf[OSC_MAX_BUF];
static OscMessage msg[OSC_MAX_MESSAGES];
static int bufpos;
static int msgpos;

int OscMatch(const char *pat, const char *addr) {    // TODO add // wildcard support
    for (int n=0;*pat;addr++,pat++) switch (*pat) {
    default:  if (*pat!=*addr) return 0; break;
    case '?': break;
    case '[': n=(*++pat=='!'); for (pat+=n; *pat!=']' && *pat;++pat) { if (pat[1]=='-') { if (*addr>=*pat && *addr<=pat[2]) { n=!n; break; } pat+=pat[2] ? 2 : 1; } else if (*pat==*addr) { n=!n; break; } }
              if (!n) return 0; while (*pat && *pat!=']') pat++; break;
    case '{': n=0; for (const char *p=++pat; *p && *p!='}' && *p!='/'; pat=++p) { while (*p && *p!='}' && *p!='/' && *p!=',') p++; if (!strncmp(pat,addr,p-pat)) { addr+=p-pat; n=1; break; } }
              while (*pat && *pat!='}') pat++; if (!n) return 0; break;
    case '*': while (pat[1]=='*') pat++; n=0; if (pat[1]=='/' || pat[1]==0) { while (*addr && *addr!='/') ++addr; n=1; } else for (;*addr!='/' && *addr; ++addr) if (OscMatch(pat + 1, addr)) { n=1; break; }
              if (!n) return 0; addr--; break;
    }
    return *addr== 0;
}

void TestOscMatch() { // lol 'test'
    assert(1==OscMatch("/[a-c]/?/[abc]/*/fish*/*food/f*/{foo,bar,baz}","/a/b/c//fishfood/monkeyfood/f/bar"));
    assert(0==OscMatch("/[a-c]/?/[abc]/*/fish*/*food/f*/{fog,bar,baz}","/a/b/c//fishfood/monkeyfood/f/foo"));
    assert(0==OscMatch("/[a-c]/?/[abc]/*/fith*/*food/f*/{foo,bar,baz}","/a/b/c//fishfood/monkeyfood/f/foo"));
    assert(0==OscMatch("/[a-c]/?/[abc]/*/fish*/*good/f*/{foo,bar,baz}","/a/b/c//fishfood/monkeyfood/f/foo"));
    assert(0==OscMatch("/[a-c]/?/[abc]/*/fish*/*food/g/{foo,bar,baz}","/a/b/c//fishfood/monkeyfood/f/foo"));
    assert(1==OscMatch("/[fa-cd]/?/[abc]/*/fish*/*food/f*/{foo,bar,baz}","/d/b/c//fishfood/monkeyfood/f/bar"));
}

const OscMessage *OscGetAll(const OscMessage **last_out) {
    *last_out=msg+msgpos;
    return msg;
}

const OscMessage *OscFind(const char *addr) { // search in reverse order, so newest wins
    for (int i=msgpos;i-->0;) if (OscMatch(msg[i].pattern, addr)) return &msg[i];
    return 0;
}

int OscParse(OscMessage *out, int maxmsg, const char *s, const char *e);

int OscUpdate(int fd) {
    if (fd<0) return 0;
    for (msgpos=0,bufpos=0;msgpos<OSC_MAX_MESSAGES && bufpos<OSC_MAX_BUF-8;) {
        sockaddr_in addr;
        int addrlen = sizeof(addr);
        int n=recvfrom(fd,buf+bufpos,OSC_MAX_BUF-bufpos-1,0,(struct sockaddr*)&addr,&addrlen); 
        if (n<=0)  // TODO - look at what the error was
            return 0;
        char *s=buf+bufpos; 
        s[n]=0; // null terminate packet always, for easier c handling
        msgpos+=OscParse(msg+msgpos,OSC_MAX_MESSAGES-msgpos,s,s+n);    
        bufpos+=n+1;                
    }   
    return 1;
}

float asfloat(uint32_t x)   { union { float f;  uint32_t i; } u; u.i=x; return u.f; }
double asdouble(uint64_t x) { union { double f; uint64_t i; } u; u.i=x; return u.f; }

int OscParseInt32(const char **s, const char *e) {
    if (*s+4>e) { *s=e; return 0; }
    int rv=htonl(*(uint32_t*)*s);
    *s+=4;
    return rv;
}
int64_t OscParseInt64(const char **s, const char *e) {
    if (*s+8>e) { *s=e; return 0; }
    int rv=htonl(*(uint64_t*)*s);
    *s+=8;
    return rv;
}
const char *OscParseString(const char **s, const char *e) {
    int len=strlen(*s);
    const char *rv=*s;
    *s=rv+((len+4)&~3); 
    if (*s>e) *s=e;
    return rv;
}
const char *OscParseBlob(const char **s, const char *e, int *len) {
    *len=OscParseInt32(s,e); 
    int maxlen=e-*s;
    if (*len>maxlen) *len=maxlen;
    if (*len<0) *len=0;
    const char *rv=*s;
    *s=rv+((*len+3)&~3);
    if (*s>e) *s=e;
    return rv;
}
int OscParse(OscMessage *out, int maxmsg, const char *s, const char *e) {
    if (maxmsg<=0 || s>=e) return 0;
    if (*(uint64_t*)s==*(uint64_t*)"#bundle\0") { // bundle is #bundle, uint64_t time, uint32_t length, <osc packet>        
        OscParseInt64(&s,e); // skip time for now. TODO
        int msgcount=0;
        while (s<e && msgcount<maxmsg) {
            int len=OscParseInt32(&s,e);
            int maxlen=e-s;
            if (len>maxlen) len=maxlen;
            if (len<0) len=0;
            int n=OscParse(out+msgcount, maxmsg-msgcount, s, s+len);
            msgcount+=n;
            s+=((len+3)&~3);
        }
        return msgcount;
    }
    // single message
    memset(out,0,sizeof(OscMessage));
    out->pattern=OscParseString(&s,e);
    if (!out->pattern)
        return 0;
    out->types=(*s==',')?OscParseString(&s,e):",f";
    if (!out->types) 
        return 0;
    out->types++;
    out->data=s;
    for (int param=0;param<4;++param) {
        int f2i=0;
        switch (out->types[param]) {
            default: return 1; // done!
            case OSC_CHAR: case OSC_RGBA: // all int32... 
            case OSC_MIDI: case OSC_INT:        out->i[param]=OscParseInt32(&s,e); break;
            case OSC_TIME: case OSC_INT64:      out->i[param]=OscParseInt64(&s,e); break;
            case OSC_STRING: case OSC_SYMBOL:   out->s[param]=OscParseString(&s,e); out->i[param]=strlen(out->s[param]); break;
            case OSC_FLOAT:                     out->f[param]=asfloat(OscParseInt32(&s,e)); f2i=1; break;
            case OSC_DOUBLE:                    out->f[param]=asdouble(OscParseInt32(&s,e)); f2i=1; break;
            case OSC_BLOB:                      {int len=0; out->s[param]=OscParseBlob(&s,e,&len); out->i[param]=len; break; }
            case OSC_INFINITUM:                 out->f[param]=INFINITY; out->i[param]=0x7fffffff; break;
            case OSC_TRUE:                      out->i[param]=1; break;
            case OSC_FALSE: case OSC_NIL:       out->i[param]=0; break;
//          case OSC_ARRAY:     TODO
        }
        if (f2i) 
            out->i[param]=(int64_t)(out->f[param]+0.5f);
        else
            out->f[param]=out->i[param];
    }   
    return 1;
}

#endif


#ifdef OSCRECV_DEMO
#include <errno.h>
#include <stdio.h>
int main() {
    WSADATA wsadata = {0};
    WSAStartup(MAKEWORD(2,2),&wsadata);

    SOCKET fd;
    struct addrinfo* addr = 0;
    struct addrinfo hints = {0};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_ADDRCONFIG;
    
    if( getaddrinfo("127.0.0.1", "9000", &hints, &addr) != 0 ) {
        return printf("failed to resolve remote socket address"), -__LINE__;
    }
    fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if( fd < 0 ) {
        return printf("failed to open socket %s\n", strerror(errno)), -__LINE__;
    }

    if (bind(fd,addr->ai_addr,addr->ai_addrlen)==-1) {
        return printf("%s",strerror(errno)), -__LINE__;
    }

    u_long nonblocking_enabled = TRUE;
    ioctlsocket( fd, FIONBIO, &nonblocking_enabled );

    while(1) {
        static unsigned char counter = 0;
        printf("\r%c listening to 9000", "\\|/-"[ counter = (counter+1) & 3 ]);

        Sleep(100);

        OscUpdate(fd); // read the udp port and parse all messages found there

        const OscMessage *begin, *end = 0;
        begin = OscGetAll(&end); // return all messages parsed by OscUpdate
        int num = end - begin;

        for( ; begin && begin < end ; ) {
            while( begin < end ) {
                printf("> %s\n", begin->pattern);
                begin++;
            }
        }
    }

    freeaddrinfo(addr);

    return 0;
}
#endif
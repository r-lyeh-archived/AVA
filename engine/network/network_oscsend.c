// based on midi2osc hack by @mmalex (public domain).
// - rlyeh, public domain

int osc_open( const char *host, const char *port );
int osc_send( int s, const char *msg, int msg_len );
int osc_close( int s );



#ifdef OSCSEND_C
#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h> // for sockets
#include <ws2tcpip.h> // for getaddrinfo
#pragma comment(lib,"ws2_32.lib") // for sockets

struct udp_socket {
    SOCKET fd;
    struct addrinfo* addr;
};

static int udp_l = 1;
static struct udp_socket udp_list[128+1];

int osc_open( const char *host, const char *port ) {
    static WSADATA wsadata = {0}, *init = 0; if(!init) { init = &wsadata;
    WSAStartup(MAKEWORD(2,2),&wsadata);
    }

    struct udp_socket u = {0};
    struct addrinfo hints = {0};
    hints.ai_family = AF_UNSPEC; // select ip4 or ip6. explicit: ip4->AF_INET ip6->AF_INET6
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_ADDRCONFIG;
    
    if( getaddrinfo(host, port, &hints, &u.addr) != 0 ) {
        // failed to resolve remote socket address
        return 0;
    }
    u.fd = socket(u.addr->ai_family, u.addr->ai_socktype, u.addr->ai_protocol);
    if( u.fd < 0 ) {
        // "failed to open socket: %s", strerror(errno)
        return 0;
    }

    // assert(udp_l < 128); @todo: fix limits
    udp_list[ udp_l ] = u;
    return udp_l++;
}

int osc_send( int s, const char *msg, int msg_len ) {
    int sent;
    struct udp_socket *u = &udp_list[ s ];
    for( ;; ) {
        int res = sendto(u->fd, msg, msg_len, 0, u->addr->ai_addr, u->addr->ai_addrlen);
#ifdef _WIN32
        if (res == -1 && WSAGetLastError() == WSAEINTR) continue;
        else sent = res;
#else
        if (res == -1 && errno == EINTR) continue;
        else sent = res;
#endif
        break;
    }
    return sent;
}

int osc_close( int s ) {
    struct udp_socket *u = &udp_list[s];
    freeaddrinfo(u->addr);
    return 0;
}

#endif


#ifdef OSCSEND_DEMO
#include <omp.h>

int main( int argc, char **argv ) {

    if( argc <= 1 ) {
        return printf("%s /osc/address [ip] [port]\n", argv[0]), -__LINE__;
    }

    const char *oscaddr = argv[1]; 
    const char *ip = argc > 2 ? argv[2]  : "127.0.0.1";
    const char *port = argc > 3 ? argv[3] : "9000";

    int fd = osc_open(ip, port);
    if( fd > 0 ) {
        printf("benchmarking...");
        double t = -omp_get_wtime();
        #ifndef N
        #define N 1
        #endif
        char buf[1024];
        enum { M = 1000*1000 };
        for( int i = 0; i < N*M; ++i) {
            osc_send(fd, buf, 1024);
        }
        t += omp_get_wtime();
        printf("%dM pkts sent in %5.2fs, %5.2fM pkt/s, %5.2fMB/s\n", N, t, (N*M)/t, ((N*M)*1024ULL/t)/1024/1024);
    }
    if( fd > 0 ) {
        printf("sending to %s:%s%s%s, ctrl-z to quit\n", ip, port, oscaddr[0] == '/' ? "" : "/", oscaddr);
        printf("/");
        for( char prompt[256]; !feof(stdin) && fgets(prompt, 256, stdin); ) {
            char *no_cr = strchr(prompt, '\r'); if(no_cr) *no_cr = 0;
            char *no_lf = strchr(prompt, '\n'); if(no_lf) *no_lf = 0;
            char oscmsg[128] = {0};
            if( oscaddr[0] != '/' ) {
                strcat(oscmsg, "/");
            }
            strcat(oscmsg, oscaddr);
            strcat(oscmsg, "/");
            strcat(oscmsg, prompt);

            int sent = osc_send(fd, oscmsg, strlen(oscmsg));
            if( sent < 0 ) {
                printf("failed to send udp packet! (%d bytes: %s)\n/", (int)strlen(oscmsg), oscmsg);
            } else {
                printf("ok (%d bytes: %s)\n/", sent, oscmsg);
            }
        }

        osc_close(fd);
    }

    return 0;
}

/*
    note to self: if I ever have a midi controller, this is the way to forward messages:

    UINT devid=atoi(GetIniSetting("Device","0"));
    UINT numdevs=midiInGetNumDevs();
    for (UINT d=0;d<numdevs;++d) {
        MIDIINCAPS caps;
        if (midiInGetDevCaps(d,&caps, sizeof(caps))==MMSYSERR_NOERROR) {
            printf("%c%d: %s\n",d==devid?'*':' ',d,caps.szPname);
        }
    }
    HMIDIIN hmidi={};
    if (midiInOpen(&hmidi, devid, (DWORD_PTR)midicb, 0, CALLBACK_FUNCTION)!=MMSYSERR_NOERROR) return printf("failed to midiInOpen\n");      
    if (midiInStart(hmidi)!=MMSYSERR_NOERROR) return printf("failed to midiInStart\n");     
    while (_getch()!=27);
    midiInStop(hmidi);
    midiInClose(hmidi);
    return 0;
*/

#endif

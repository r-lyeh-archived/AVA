// based on midi2osc hack by @mmalex (public domain).
// - rlyeh, public domain

#ifdef OSCSEND_DEMO

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h> // for sockets
#include <ws2tcpip.h> // for getaddrinfo
#pragma comment(lib,"ws2_32.lib") // for sockets

int main( int argc, char **argv ) {
    WSADATA wsadata = {0};
    WSAStartup(MAKEWORD(2,2),&wsadata);

    if( argc <= 1 ) {
        return printf("%s /osc/address [ip] [port]\n", argv[0]), -__LINE__;
    }

    const char *oscaddr = argv[1]; 
    const char *ip = argc > 2 ? argv[2]  : "127.0.0.1";
    const char *port = argc > 3 ? argv[3] : "9000";
    printf("sending to %s:%s%s%s, ctrl-z to quit\n", ip, port, oscaddr[0] == '/' ? "" : "/", oscaddr);

    SOCKET fd;
    struct addrinfo* addr = 0;
    struct addrinfo hints = {0};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_ADDRCONFIG;
    
    if( getaddrinfo(ip, port, &hints, &addr) != 0 ) {
        return printf("failed to resolve remote socket address"), -__LINE__;
    }
    fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if( fd < 0 ) {
        return printf("failed to open socket %s\n", strerror(errno)), -__LINE__;
    }

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
        int sent = sendto(fd, oscmsg, strlen(oscmsg), 0, addr->ai_addr, addr->ai_addrlen);
        if( sent < 0 ) {
            printf("failed to send udp packet! (%d bytes: %s)\n/", (int)strlen(oscmsg), oscmsg);
        } else {
            printf("ok (%d bytes: %s)\n/", sent, oscmsg);
        }
    }

    freeaddrinfo(addr);
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

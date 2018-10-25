#ifndef NETCOMPAT_H
#define NETCOMPAT_H

#include <sys/types.h>

#ifdef _WIN32

#   include <winsock2.h>
#   include <ws2tcpip.h>
#   include <windows.h>

#   ifndef _MSC_VER
    static
    const char* inet_ntop(int af, const void* src, char* dst, int cnt){
        struct sockaddr_in srcaddr;
        memset(&srcaddr, 0, sizeof(struct sockaddr_in));
        memcpy(&(srcaddr.sin_addr), src, sizeof(srcaddr.sin_addr));
        srcaddr.sin_family = af;
        if (WSAAddressToString((struct sockaddr*) &srcaddr, sizeof(struct sockaddr_in), 0, dst, (LPDWORD) &cnt) != 0) {
            DWORD rv = WSAGetLastError();
            return NULL;
        }
        return dst;
    }
#   endif

#   pragma comment(lib,"ws2_32.lib")

#   define INIT()                    do { static WSADATA wsa_data; static int i = 0; if(!i) { i = WSAStartup( MAKEWORD(2, 2), &wsa_data ); i = 1; } } while(0)
#   define SOCKET(A,B,C)             socket((A),(B),(C))
#   define ACCEPT(A,B,C)             accept((A),(B),(C))
#   define CONNECT(A,B,C)            connect((A),(B),(C))
#   define CLOSE(A)                  closesocket((A))
#   define RECV(A,B,C,D)             recv((A), (char *)(B), (C), (D))
#   define READ(A,B,C)               recv((A), (char *)(B), (C), (0))
#   define SELECT(A,B,C,D,E)         select((A),(B),(C),(D),(E))
#   define SEND(A,B,C,D)             send((A), (const char *)(B), (int)(C), (D))
#   define WRITE(A,B,C)              send((A), (const char *)(B), (int)(C), (0))
#   define GETSOCKOPT(A,B,C,D,E)     getsockopt((A),(B),(C),(char *)(D), (int*)(E))
#   define SETSOCKOPT(A,B,C,D,E)     setsockopt((A),(B),(C),(char *)(D), (int )(E))

#   define BIND(A,B,C)               bind((A),(B),(C))
#   define LISTEN(A,B)               listen((A),(B))
#   define SHUTDOWN(A)               shutdown((A),2)
#   define SHUTDOWN_R(A)             shutdown((A),0)
#   define SHUTDOWN_W(A)             shutdown((A),1)
#   define SLEEP(SS)                 Sleep((SS)*1000)

    // fill missing api

    typedef int socklen_t;

    enum {
        F_GETFL = 0,
        F_SETFL = 1,

        O_NONBLOCK = 128 // dummy
    };

    int fcntl( int sockfd, int mode, int value ) {
        // get socket status flags
        if( mode == F_GETFL ) {
            return 0; // original return current sockfd flags
        }
        // set socket status flags
        if( mode == F_SETFL ) { 
            u_long iMode = ( value & O_NONBLOCK ? 0 : 1 );
            int result = ( ioctlsocket( sockfd, FIONBIO, &iMode ) == NO_ERROR );
            return 0; // result?
        }
        return 0;
    }

#else

#   include <fcntl.h>
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <netdb.h>
#   include <unistd.h>    //close

#   include <arpa/inet.h> //inet_addr, inet_ntop
//# include <netinet/in.h>

#   define INIT()                    do {} while(0)
#   define SOCKET(A,B,C)             socket((A),(B),(C))
#   define ACCEPT(A,B,C)             accept((A),(B),(C))
#   define CONNECT(A,B,C)            connect((A),(B),(C))
#   define CLOSE(A)                  close((A))
#   define READ(A,B,C)               read((A),(B),(C))
#   define RECV(A,B,C,D)             recv((A), (void *)(B), (C), (D))
#   define SELECT(A,B,C,D,E)         select((A),(B),(C),(D),(E))
#   define SEND(A,B,C,D)             send((A), (const int8 *)(B), (C), (D))
#   define WRITE(A,B,C)              write((A),(B),(C))
#   define GETSOCKOPT(A,B,C,D,E)     getsockopt((int)(A),(int)(B),(int)(C),(      void *)(D),(socklen_t *)(E))
#   define SETSOCKOPT(A,B,C,D,E)     setsockopt((int)(A),(int)(B),(int)(C),(const void *)(D),(int)(E))

#   define BIND(A,B,C)               bind((A),(B),(C))
#   define LISTEN(A,B)               listen((A),(B))
#   define SHUTDOWN(A)               shutdown((A),SHUT_RDWR)
#   define SHUTDOWN_R(A)             shutdown((A),SHUT_RD)
#   define SHUTDOWN_W(A)             shutdown((A),SHUT_WR)
#   define SLEEP(SS)                 sleep((SS))

#endif // _WIN32

#endif // NETCOMPAT_H

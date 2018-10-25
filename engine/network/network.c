#ifdef NETWORK_C
#pragma once
#define SOCKET_C
#define DISPATCH_C
#define PORT_C
#define WEBSERVER_C
#define OSCSEND_C
#define OSCRECV_C
#endif

#include "network_compat.c"
#include "network_socket.c"
#include "network_dispatch.c"
#include "network_port.c"
#include "network_webserver.c"
#include "network_oscsend.c"
#include "network_oscrecv.c"

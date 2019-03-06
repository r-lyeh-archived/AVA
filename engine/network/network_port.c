// Get deterministic assigned ports based on hash name of service (concept from str2port repo).
// Our algorithm: hash(name of service) -> split hash into {1024..63000} ports.
// - rlyeh, public domain

#ifndef PORT_H
#define PORT_H

int  network_port( const char *service, int slot /*0..7*/ );

#endif

// ----------------------------------------------------------------------------

#ifdef PORT_C
#pragma once
#include <stdint.h>

int network_port( const char *service_name, int slot ) {
    // see also: https://en.wikipedia.org/wiki/Ephemeral_port
    // excluded ranges: 32768..61000 (linux), 49152..65535 (freebsd+vista+win7), 1025..5000 (winsrv2003+bsd)
    // output range: [5001..32724], in 4096 steps

    // hash64
    uint64_t sid = 0;
    while( *service_name++ ) sid = sid ^ (service_name[-1] * 131);

    // splitmix64
    uint64_t h = (sid += UINT64_C(0x9E3779B97F4A7C15));
    h = (h ^ (h >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    h = (h ^ (h >> 27)) * UINT64_C(0x94D049BB133111EB);
    h = (h ^ (h >> 31));

    // buckets
    int ports[8] = {0};
    for( int *p = ports; h > 0; h >>= 12 ) {
        *p++ = ((h & 0xFFF) * 677 / 100 + 5001);
    }
    return ports[ slot & 7 ];
}

#endif

#ifdef PORT_DEMO
#include <stdio.h>
void print( const char *service_name ) {
    printf("service: %-16s -> candidate ports: ", service_name);
    for( int i = 0; i < 8; ++i) {
        printf("%5d,%c", network_port( service_name, i), i == 7 ? '\n' : ' ' );
    }
}
int main() {
    print("ProxyRedirector");
    print("Gateway");
    print("Paywall");
    print("LobbyServer");
    print("MatchMaking");
}
#endif

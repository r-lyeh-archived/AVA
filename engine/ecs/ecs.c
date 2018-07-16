// fast simple ecs
// - rlyeh, public domain
//
// features:
// - mostly heap allocation free
// - unlimited entities
// - 2^64-1 systems max
//
// cons:
// - 64 components max

#ifndef ECS_H
#define ECS_H

#include "../core/array.c"
#include <stdint.h>
#include <stdio.h>

typedef union component component;

union component {
#ifdef ECS_DEMO
         struct position  { float x,y,z; };    /*c0*/
         struct velocity  { float vx,vy,vz; }; /*c1*/
         struct color     { float r,g,b; };    /*c2*/
         struct health    { float health; };   /*c3*/
#endif
#ifdef   COMPONENT_DATA
         COMPONENT_DATA
#endif
#ifdef   COMPONENT_HEADER
#include COMPONENT_HEADER
#endif
         int dummy;
};

/* api */

#define component(e,c)  components(e)[c]
#define foreach(obj, ...) \
    for( int s = new_system(__VA_ARGS__), e = 0; e < w.ne; ++e ) \
        for( component **obj = s == (s & w.entities[e * w.nc + 0]) ? components(e) : 0; obj ; obj = 0 )

/* internals */

int new_system( int arg1, ... );
int new_entity( int cid1, ... );
component** components( int eid );
void world_dump( FILE *fp, int system );

struct world {
    int ne, nc;                  // ne: number of entities, nc: number of components (stride) (cN)
    array(component) instanced;  // instanced components
    array(int) entities;         // components table per entity [ e1(cflags,c1,c2..nc) e2(cflags,c1,c2..nc) .. eN(cflags,c1,c2..nc) ]
    array(int) registered;       // registered components, also sizeof(registered) == stride(entities)
};

extern struct world w;

/* syntax sugar */

#define new_system(...) new_system(__VA_ARGS__, -1)
#define new_entity(...) new_entity(__VA_ARGS__, -1)
#define world_dump(fp,...) world_dump(fp, new_system(__VA_ARGS__, -1))

#endif


#ifdef ECS_C
#pragma once
#include <stdint.h>
#include <stdarg.h>

static struct world w = {0};

static void register_component(int cid) {
    for (int i = 0; i < array_count(w.registered); ++i) {
        if (w.registered[i] == cid) {
            return;
        }
    }

    if( !w.nc ) {
        /* cflags */
        array_append( w.entities, 0 );
    }

    array(int) ec = 0;
    for( int i = 0; i < w.ne; ++i ) {
        for( int j = 0; j < w.nc; ++j ) {
            array_append(ec, w.entities[ i * w.nc + j ]);
        }
        array_append(ec, -1);
    }
    array_copy(w.entities,ec);
    array_free(ec);

    array_append(w.registered, cid);
    w.nc ++;
}

static int new_component() {
    component c = {0};
    array_append( w.instanced, c );
    return array_count( w.instanced ) - 1;
}

int (new_system)( int arg1, ... ) {
    int s = 0;
    va_list ap;
    va_start(ap, arg1); 
    for( int i = arg1; i >= 0; i = va_arg(ap, int) ) {
        s |= 1ull << (uint64_t)i;
    }
    va_end(ap);
    return s;
}

int (new_entity)( int cid1, ... ) {
    int eid = w.ne;

    va_list ap;
    va_start(ap, cid1);
    for( int cid = cid1; cid >= 0; cid = va_arg(ap, int) ) {
        register_component(cid);
    }
    va_end(ap);

    for (int i = 0; i < w.nc; ++i) {
        array_append(w.entities, -1);
    }

    uint64_t flags = 0;
    {
        va_list ap;
        va_start(ap, cid1);
        for( int cid = cid1; cid >= 0; cid = va_arg(ap, int) ) {
            w.entities[ eid * w.nc + cid ] = new_component();
            flags |= 1ull << (uint64_t)cid;
        }
        va_end(ap);
    }
    w.entities[ eid * w.nc + 0 ] = flags;

    eid = w.ne++;
    return eid;
}

component** components( int eid ) {
    static component* local[64];
    for( int i = 0; i < w.nc; ++i ) {
        int cid = w.entities[ eid * w.nc + i ];
        local[ i ] = cid >= 0 ? &w.instanced[ cid ] : 0;
    }
    return local;
}

void (world_dump)( FILE *fp, int smask ) {
    for( int i = 0; i < w.ne; ++i ) {
        int s = w.entities[ i * w.nc + 0 ];
        if( smask && (smask != (s & smask))) continue;
        fprintf(fp, "e%02d s%02d ", i, s);
        for( int j = 1; j < w.nc; ++j ) {
            int cid = w.entities[ i * w.nc + j ];
            if( cid >= 0 ) {
                fprintf(fp, "c%d (void*)[%p (i%d)] ", j, &w.instanced[ cid ], cid );
            }
        }
        fputc('\n', fp);
    }
}

#endif

#ifdef ECS_DEMO
#include <stdio.h>
#include <assert.h>

#ifdef _MSC_VER
#include <omp.h>
#define clock_t double
#define clock omp_get_wtime
#define CLOCKS_PER_SEC 1.0
#else
#include <time.h>
#endif

int main(int argc, char **argv) {

    /* entities to spawn */
    #ifndef N
    int N = argc > 1 ? atoi(argv[1]) : 100000;
    #endif

    /* frames to benchmark */
    #ifndef F
    int F = argc > 2 ? atoi(argv[2]) : 1000;
    #endif

    // declare components: c1, c2, ...
    enum { POSITION, VELOCITY, COLOR, HEALTH, INPUT };

    // spawn entities
    int player = new_entity(POSITION, VELOCITY, HEALTH, INPUT);
    component(player, VELOCITY)->vx = 1;
    component(player, VELOCITY)->vy = 2;

    for (int i = 0; i < N; ++i) {
        switch (i & 3) {
            break; case 0:; /* static enemy */
            int enemy0 = new_entity(POSITION, COLOR, HEALTH);
            break; case 1:; /* dynamic enemy */
            int enemy1 = new_entity(POSITION, COLOR, HEALTH, VELOCITY);
            break; case 2:; /* static light */
            int light0 = new_entity(POSITION, COLOR);
            break; case 3:; /* dynamic light */
            int light1 = new_entity(POSITION, COLOR, VELOCITY);
        }
    }

    // process & benchmark 
    clock_t start = clock();

    for( int frame = 0; frame < F; ++frame ) {
        foreach(obj, POSITION, VELOCITY) {
            component *p = obj[POSITION];
            component *v = obj[VELOCITY];
            p->x += v->vx;
            p->y += v->vy;
            p->z += v->vz;
        }
    }

    clock_t end = clock();
    double t = (end - start) / CLOCKS_PER_SEC * 1.0;

    // verify
    // world_dump(stdout, INPUT);

    {component *p = components(player)[POSITION];
     printf("eid:%d (position: %f,%f,%f)\n", player, p->x, p->y, p->z );}

    assert( components(player)[POSITION]->x == (F * 1));
    assert( components(player)[POSITION]->y == (F * 2));

    // stats
    int num_phys_entities = 0;
    foreach(obj, POSITION, VELOCITY) {
        ++num_phys_entities;
    }

    int T = F * num_phys_entities;
    printf("%d frames * %d num_phys_entities = %d total ops, in %.3fs => %.3fM ops/s, %.2fms/frame\n", 
        F, num_phys_entities, T, t, (T / 1000000.0) / t, (t * 1000 / F) );
}

#endif

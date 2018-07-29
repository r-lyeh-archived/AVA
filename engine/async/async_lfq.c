// # lockfree queues (multiple consumer-multiple producer) ####################
// License: WTFPL. https://github.com/darkautism/lfqueue
// Use -O0 flag to compile (needed?).

#ifndef LFQ_H
#define LFQ_H

struct lfq;

int   lfq_new(struct lfq *ctx);
int   lfq_del(struct lfq *ctx);
int   lfq_add(struct lfq *ctx, void *data);
void *lfq_pop(struct lfq *ctx );

#endif

#ifdef LFQ_C
#pragma once
#include "../core/core_realloc.c"
#include "async_atomic.c"
#include <stdlib.h> 
#include <string.h>
#include <errno.h>

struct lfq_node {
    void * data;
    struct lfq_node *next;
};

struct lfq {
    struct lfq_node *head;
    struct lfq_node *tail;
    size_t count; // int
};

int lfq_new(struct lfq *ctx) {
    struct lfq_node * tmpnode = memset( (char*)REALLOC(0,sizeof(struct lfq_node)), 0, sizeof(struct lfq_node));
    if (!tmpnode) 
        return -errno;
    
    memset(ctx,0,sizeof(struct lfq));
    ctx->head=ctx->tail=tmpnode;
    return 0;
}

int lfq_del(struct lfq *ctx){
    if ( ctx->tail && ctx->head ) { // if have data in queue
        struct lfq_node * walker = ctx->head, *tmp;
        while ( walker != ctx->tail ) { // while still have node
            tmp = walker->next;
            REALLOC(walker, 0);
            walker=tmp;
        }
        REALLOC(ctx->head, 0); // free the empty node
        memset(ctx,0,sizeof(struct lfq));
    }
    return 0;
}

int lfq_add(struct lfq *ctx, void * data) {
    struct lfq_node * p;
    struct lfq_node * tmpnode = memset( (char*)REALLOC(0,sizeof(struct lfq_node)), 0, sizeof(struct lfq_node));
    if (!tmpnode)
        return -errno;
    
    tmpnode->data=data;
    do {
        p = ctx->tail;
        if ( __sync_bool_compare_and_swap(&ctx->tail,p,tmpnode)) {
            p->next=tmpnode;
            break;  
        }
    } while(1);
    __sync_add_and_fetch( &ctx->count, 1);
    return 0;
}

void * lfq_pop(struct lfq *ctx ) {
    void * ret=0;
    struct lfq_node * p;
    do {
        p = ctx->head;
    } while(p==0 || !__sync_bool_compare_and_swap(&ctx->head,p,0));
    
    if( p->next==0) {
        ctx->head=p;
        return 0;
    }
    ret=p->next->data;
    ctx->head=p->next;
    __sync_add_and_fetch( &ctx->count, -1);
    REALLOC(p, 0);
    return ret;
}

#endif

#if LFQ_DEMO
#include <stdio.h>
#include <stdlib.h>

int main() {
    long ret;
    struct lfq ctx;
    lfq_new(&ctx);
    lfq_add(&ctx,(void *)1);
    lfq_add(&ctx,(void *)3);
    lfq_add(&ctx,(void *)5);
    lfq_add(&ctx,(void *)8);
    lfq_add(&ctx,(void *)4);
    lfq_add(&ctx,(void *)6);

    while ( (ret = *(int*)lfq_pop(&ctx)) != 0 )
        printf("lfq_dequeue %ld\n", ret);

    lfq_del(&ctx);
}
#endif

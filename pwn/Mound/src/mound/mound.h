#pragma once
#define MCACHEBINS 24
#define MAX_CHUNKS 0x1000
#define MOUND_SIZE 0x400000
void moundsetup();
unsigned long rand64bit();
void* mmalloc(unsigned int size);
void* top_chunk_alloc(unsigned int size);
void* mcache_alloc(unsigned int size);
typedef unsigned long ulong;
struct top_chunk {
    unsigned long id;
    unsigned long size;
};
struct mcache_carrier{
    char sizes[MCACHEBINS];
    void* bins[MCACHEBINS];
};

struct mound_main_struct {
    void * moundlocation;
    unsigned long trusted_ids[MAX_CHUNKS];
    struct mcache_carrier* mcache;
    struct top_chunk* top;
};

struct allocated_chunk {
    unsigned long id;
    unsigned long size;
    char data[];
};
struct mcache_chunk {
    unsigned long id;
    unsigned long size;
    struct mcache_carrier* verify;
    struct mcache_chunk* next;
    char data[];
};

#define MOUNDBASE (struct mound_main_struct*) 0xdead0000000
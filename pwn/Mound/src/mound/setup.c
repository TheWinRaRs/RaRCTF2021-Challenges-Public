#include "mound.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#define MMAP(addr, size, prot, flags) \
 mmap((addr), (size), (prot), (flags)|MAP_ANONYMOUS|MAP_PRIVATE, -1, 0)
void moundsetup(){
    // Avoid using glibc by all possible means
    srand(time(NULL) ^ getpid());
    // Set up the mound by mmaping a small region for the mound main struct
    void* main_segment = MMAP((void *)0xdead0000000,sizeof(struct mound_main_struct),PROT_READ|PROT_WRITE,0);
    if (main_segment == (void *)MAP_FAILED){
        puts("Mound failed. ABORT!");
        exit(1);
    }
    // Create the actual mound
    // Allocate at 0xbeef<<28 with size of 0x400000
    void * mound = MMAP((void *)0xbeef0000000,MOUND_SIZE,PROT_READ|PROT_WRITE,0);
    if (mound == (void *)MAP_FAILED){
        puts("Mound failed. ABORT!");
        exit(1);
    }
    struct mound_main_struct* mound_main = MOUNDBASE;
    mound_main->moundlocation = mound;
    mound_main->top = mound;
    struct top_chunk * firstchunk = mound;
    firstchunk->id = rand64bit();
    mound_main->trusted_ids[0] = firstchunk->id;
    firstchunk->size = MOUND_SIZE;
    mound_main->mcache = top_chunk_alloc(sizeof(struct mcache_carrier)); // mcache carrier structure is at the top of the mound, first allocation
}

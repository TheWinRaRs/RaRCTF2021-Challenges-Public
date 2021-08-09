#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mound.h"
#define align(size) size + 16 + ((16 - (size % 16)) % 16)
#define get_mcache_index(size) size/16 - 2

void* top_chunk_alloc(unsigned int size){
    struct mound_main_struct* mound_main = MOUNDBASE;
    struct top_chunk* top = mound_main->top;
    ulong topsize = top->size;
    size = align(size);
    if (topsize <= size + 0x20){
        puts("Mound ran out of memory. Extra mmap not supported.");
        exit(0);
    }
    struct top_chunk* newtop = (struct top_chunk *)((unsigned long)top + size); //Calculate new top chunk location
    memcpy(newtop,top,sizeof(struct top_chunk));
    newtop->size -= size;
    mound_main->top = newtop;
    // Create new chunk
    struct allocated_chunk* toreturn = (struct allocated_chunk *)top; // Goes where the old top chunk was
    toreturn->id = rand64bit();
    toreturn->size = size;
    return toreturn->data;
}
void* remove_id(unsigned long id){
    struct mound_main_struct* mound_main = MOUNDBASE;
    for(int i = 0; i < MAX_CHUNKS; i ++){
        if (mound_main->trusted_ids[i] == id){
            mound_main->trusted_ids[i] = 0;
            return;
        }
    }
}
void* mcache_alloc(unsigned int size){
    struct mound_main_struct* mound_main = MOUNDBASE;
    struct mcache_carrier* mcache = mound_main->mcache;
    size = align(size);
    int index = get_mcache_index(size);
    struct mcache_chunk* bin = mcache->bins[index];
    // Bin points to first chunk, which we steal
    // We set the bin in mcache bins to whatever this bin points to
    mcache->bins[index] = bin->next;
    mcache->sizes[index] -= 1;
    if (bin->verify != mcache) {
        puts("Mcache: Invalid verify");
        exit(1);
    }
    bin->next = 0;
    bin->verify = 0;

    struct allocated_chunk* toreturn = (struct allocated_chunk *)bin;
    remove_id(toreturn->id);
    return toreturn->data;

}
void* mmalloc(unsigned int size){
    // Decide method of allocation, then allocate
    struct mound_main_struct* mound_main = MOUNDBASE;
    struct mcache_carrier* mcache = mound_main->mcache;
    int newsize = align(size);
    int index = get_mcache_index(newsize);
    if(index < MCACHEBINS && mcache->bins[index] != NULL){
        return mcache_alloc(size);
    }
    else{
        return top_chunk_alloc(size);

    }
}
unsigned long mmalloc_usable_size(char* ptr){
    struct allocated_chunk* chunk = (ptr - sizeof(unsigned long) - sizeof(unsigned long));
    return chunk->size - 0x10;
}
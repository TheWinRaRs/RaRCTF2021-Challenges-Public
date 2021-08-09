#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mound.h"
#define align(size) size + 16 + ((16 - (size % 16)) % 16)
#define get_mcache_index(size) size/16 - 2
void register_id(unsigned long id){
    struct mound_main_struct* mound_main = MOUNDBASE;
    for(int i = 0; i < MAX_CHUNKS; i ++){
        if (mound_main->trusted_ids[i] == 0){
            mound_main->trusted_ids[i] = id;
            return;
        }
    }
    puts("Mound out of space for free chunks.");
    exit(1);
}
void find_id(unsigned long id){
   struct mound_main_struct* mound_main = MOUNDBASE;
    for(int i = 0; i < MAX_CHUNKS; i ++){
        if (mound_main->trusted_ids[i] == id){
            puts("Mound: Double free detected");
            exit(1);
        }
    }
}
void mcache_free(struct mcache_chunk* chunk){
    struct mound_main_struct* mound_main = MOUNDBASE;
    struct mcache_carrier* mcache = mound_main->mcache;
    unsigned int index = get_mcache_index(chunk->size);
    if(index >= 24){
        puts("Mound does not support such large freeing yet.");
        exit(1);
    }
    struct mcache_chunk* bin = mcache->bins[index];
    mcache->bins[index] = chunk;
    chunk->verify = mcache;
    chunk->next = bin;
    mcache->sizes[index] += 1;
    register_id(chunk->id);
}

void mfree(char * ptr){
    struct allocated_chunk* chunk = (ptr - sizeof(unsigned long) - sizeof(unsigned long));
    find_id(chunk->id);
    mcache_free((struct mcache_chunk*)chunk);
}

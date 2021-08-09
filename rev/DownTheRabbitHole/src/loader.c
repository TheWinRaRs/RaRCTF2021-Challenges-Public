#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#include "loader.h"
#include "headers/frames.h"


#ifdef GENXOR
static unsigned char frames[NFRAMES][LENGTH] = {FRAME0, FRAME1, FRAME2, FRAME3, FRAME4, FRAME5, FRAME6};
#else
#include "headers/transitions.h"
/*
static unsigned char transitions[NFRAMES][NFRAMES][LENGTH] = \
{
    {TRANS_0_0, TRANS_0_1, TRANS_0_2,}, 
    {TRANS_1_0, TRANS_1_1, TRANS_1_2,},
    {TRANS_2_0, TRANS_2_1, TRANS_2_2,},
};
*/
static unsigned long offsets[NFRAMES] = {FRAMEENTRY0, FRAMEENTRY1, FRAMEENTRY2, FRAMEENTRY3, FRAMEENTRY4, FRAMEENTRY5, FRAMEENTRY6};
int currentframe = 0;
#endif

void *framebase;

#ifndef GENXOR
int execute_frame(int frame) {
    // xor(BASE, BASE, transitions[currentframe][frame], LENGTH, OBFUSCATED);
    xor(BASE, BASE, transitions[currentframe][frame], LENGTH, STANDARD);
    currentframe = frame;
    func* exec = (func*)BASE + offsets[frame];
    // printf("Jumping to %p (%d)\n", exec, frame);
    int ret = exec();
    return ret;
}
#endif

int main() {
    #ifdef GENXOR
    for (int i=0; i < NFRAMES; i ++) {
        for (int j=0; j < NFRAMES; j ++) {
            unsigned char trans[LENGTH];
            xor(trans, frames[i], frames[j], LENGTH, STANDARD);
            printf("#define TRANS_%d_%d \"", i, j);
            for (int c=0; c < LENGTH; c++) {
                printf("\\x%02x", trans[c]);
            }
            puts("\"");
        }
    }
    #else
    void *framebase = mmap((void *)BASE, LENGTH, PROT_READ+PROT_WRITE+PROT_EXEC, MAP_ANONYMOUS+MAP_PRIVATE, -1,0);
    mmap((void *)SHMBASE, sizeof(struct sharedmem), PROT_READ+PROT_WRITE, MAP_ANONYMOUS+MAP_SHARED, -1,0);
    memcpy((void *)BASE, FRAME0, sizeof(FRAME0));

    printf("Will you take the (r)ed pill, or the (b)lue pill?\n> ");
    char choice = getc(stdin);
    int p;
    switch (choice) {
        case 'r':
            execute_frame(0);
            p = fork();
            if ( p < 0) {
                exit(-1);
            }
            if ( p != 0 ) {
                execute_frame(2);
            } else {
                execute_frame(1);
            }
            break;
        case 'b':
            execute_frame(3);
            p = fork();
            if ( p < 0) {
                exit(-1);
            }
            if (p == 0) {
                p = fork();
                if ( p < 0) {
                    exit(-1);
                }
                if (p == 0) {
                    execute_frame(6);
                } else {
                    execute_frame(5);
                }
            } else {
                execute_frame(4);
            }
            break;
        default:
            puts("That's not a pill!");
            break;
    }

    // Very short sleep to give decryptor time to catch up
    // Otherwise we can exit before decryption finishes
    // and output is lost
    usleep(10 * 1000);
   #endif
}

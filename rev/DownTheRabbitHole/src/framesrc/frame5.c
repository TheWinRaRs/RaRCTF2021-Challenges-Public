#include "syscalls.h"
#include "readline.h"
#include "puts.h"
#include "sleep.h"
#include "memset.h"
#include "memcmp.h"
#include "strlen.h"
#include "memcpy.h"
#include "itoa.h"
#include "../loader.h"

// Handle commands; in case of a cat, pass on to 
// the filesystem handler

void help(char *result) {
    char helpstr[] =    "help     - print this info\n"
                        "ls       - list files\n"
                        "whoami   - print username\n"
                        "cat      - read file\n"
                        "exit     - leave shell\n";
    memcpy(result, helpstr, sizeof(helpstr));
    
}

void ls(char *result) {
    char files[] = \
"drwxr-xr-x 5 tony tony    4096 Aug 14 01:06 .              \n"
"drwxr-xr-x 5 tony tony    4096 Aug  7 09:05 ..             \n"
"-rwxr-xr-x 1 tony tony    3717 Aug  9 08:42 writeup.md     \n"
"-rwxr-xr-x 1 tony tony      48 Aug 12 11:20 flag2.txt       \n"
"-rwxr-xr-x 1 tony tony     336 Aug 13 17:40 sourcecode.c   \n";
    memcpy(result, files, sizeof(files));
}

void entry() {
    sleep(50);
    char done[] = "done";
    
    struct sharedmem *shm = (struct sharedmem *)SHMBASE;
    char cmd[50];
    char result[1024];
    int res;
    char dat[8];


    do {
        memset(cmd, 0, 50);
        memset(result, 0, 1024);
        read(shm->cmdpipe[RD], cmd, 50);
        if (memcmp(cmd, "help", 4) == 0) {
            help(result);
        } else if (memcmp(cmd, "ls", 2) == 0) {
            ls(result);
        } else if (memcmp(cmd, "whoami", 6) == 0) {
            memcpy(result, "tony\n", 5);
        } else if (memcmp(cmd, "cat", 3) == 0) {
            write(shm->fnpipe[WR], cmd + 4, 46);
            read(shm->filepipe[RD], result, 1024);
        }
        else if (memcmp(cmd, "exit", 4) == 0) {
            break;
        } else {
            char unknown[] = "bash: command not found\n";
            memcpy(result, unknown, sizeof(unknown));

        }
        write(shm->cmdrespipe[WR], result, 1024);
    } while (1); 


};

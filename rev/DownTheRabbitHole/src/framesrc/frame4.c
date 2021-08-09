#include "syscalls.h"
#include "readline.h"
#include "puts.h"
#include "strlen.h"
#include "memcmp.h"
#include "memset.h"
#include "../loader.h"

// Prompt for user input, and send commands to 
// command handler

void prompt(char *user, char *cwd, char cmdbuf[50]) {
    // user@hostname:cwd$
    char colon[] = ":";
    char dollar[] = "$ ";
    puts(user);
    puts(colon);
    puts(cwd);
    puts(dollar);
    readline(cmdbuf, 50);
}

void entry() {
    struct sharedmem *shm = (struct sharedmem *)SHMBASE;
    char user[] = "tony@rabbithole";
    char cwd[20] = "/home/tony";
    char cmd[50];
    char exit[] = "Bye!\n";
    char result[1024];
    do {
        memset(cmd, 0, 50);
        memset(result, 0, 1024);
        prompt(user, cwd, cmd);
        write(shm->cmdpipe[WR], cmd, 50);
        if (memcmp(cmd, "exit", 4) == 0) {
            break;
        }
        read(shm->cmdrespipe[RD], result, 1024);
        puts(result);
    } while (1);
    puts(exit);
}

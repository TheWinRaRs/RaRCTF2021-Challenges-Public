#include "syscalls.h"
#include "puts.h"
#include "../loader.h"

// Setup pipes pre-fork

void entry() {
    struct sharedmem *shm = (struct sharedmem *)SHMBASE;
    pipe(shm->cmdpipe);
	pipe(shm->cmdrespipe);
	pipe(shm->fnpipe);
	pipe(shm->filepipe);
    char shell[] = "Welcome to my very real shell! Just read the flag now!\n";
    puts(shell);
}

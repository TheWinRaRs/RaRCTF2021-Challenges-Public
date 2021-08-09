#include "syscalls.h"
#include "readline.h"
#include "puts.h"
#include "../loader.h"
// Setup our IPC pipes and prompt for a password

void entry() {
	struct sharedmem *shm = (struct sharedmem *)SHMBASE;
	pipe(shm->pwpipe);
	pipe(shm->saltpipe);
	pipe(shm->hashpipe);
	pipe(shm->resultpipe);
	char prompt[] = "Here's a simple rev challenge: What's my password? \n> ";
	puts(prompt);
	char password[16] = {0};
    readline(password, 16);
    password[strlen(password) - 1] = 0;
    write(shm->pwpipe[WR], password, 16);
	char salt[] = "https://i.imgur.com/gub160B.png";
	write(shm->saltpipe[WR], salt, sizeof(salt));
}

#include "syscalls.h"
#include "puts.h"
#include "readline.h"
#include "md5.h"
#include "memcpy.h"
#include "hexlify.h"
#include "strlen.h"
#include "../loader.h"

// Double hash/salt input and pipe to frame2

void entry() {
	struct sharedmem *shm = (struct sharedmem *)SHMBASE;
    char userpass[16] = {0};
    char salt[32] = {0};
	read(shm->pwpipe[RD], userpass, 16);
	read(shm->saltpipe[RD], salt, 32);
    uint8_t hash1[16] = {0};
    uint8_t hash2[16] = {0};
    mbedtls_md5_ret((uint8_t*)userpass, sizeof(userpass), hash1);

    mbedtls_md5_ret((uint8_t*)salt, sizeof(salt), hash2);

    uint8_t finalbuf[32];
    memcpy(finalbuf, hash1, 16);
    memcpy(finalbuf + 16, hash2, 16);
    uint8_t hash3[16] = {0};
    mbedtls_md5_ret((uint8_t*)finalbuf, sizeof(finalbuf), hash3);


    write(shm->hashpipe[WR], hash3, 16);
}

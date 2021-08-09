#include "syscalls.h"
#include "readline.h"
#include "puts.h"
#include "sleep.h"
#include "memset.h"
#include "memcmp.h"
#include "strlen.h"
#include "memcpy.h"
#include "itoa.h"
#include "filesystem.h"
#include "aes.h"
#include "../loader.h"

// Handle file requests

static const char privwarning[] = "Hey, that's private!\n";
static const char notfound[] =  "File not found\n";
static const char flagtxt[] = "flag2.txt";
int hash(char *file) {
    char result[4] = {0};
    int a, i;
    a = strlen(file);
    file[a-1] = 0;

    while ((a = strlen(file)), (a % 4) != 0)
    {
        file[a] = 'X';
    }
    
    for (i = 0; i < a;) {
        result[0] ^= file[i++];
        result[1] ^= file[i++];
        result[2] ^= file[i++];
        result[3] ^= file[i++];
    }
    return *(int *)result;
}

void getfile(int file, char *result) {
    mbedtls_aes_context ctx;
    char key[16] = "\x64\x3e\x0a\xcb\x81\x9e\xe3\xad\x4d\x93\x76\x15\x53\xfd\x21\x04";
    char iv[16] = {0};
    mbedtls_aes_setkey_dec( &ctx, key, 128 );
    switch (file) {
        case FLAG2_TXT_ENC_HASH:
            mbedtls_aes_crypt_cbc( &ctx,
                        MBEDTLS_AES_DECRYPT,
                        framesrc_filesystem_flag2_txt_enc_len,
                        iv,
                        framesrc_filesystem_flag2_txt_enc,
                        result);

            break;
        case SOURCECODE_C_ENC_HASH:
            break;
        case WRITEUP_MD_ENC_HASH:
            break;
        default:
            memcpy(result, notfound, 16);
    }
}

void entry() {
    sleep(50);
    struct sharedmem *shm = (struct sharedmem *)SHMBASE;

    char filename[46];
    char result[1024];

    do {
        memset(filename, 0, 46);
        memset(result, 0, 1024);
        read(shm->fnpipe[RD], filename, 46);
        if (memcmp(filename, flagtxt, 9) == 0) {
            memcpy(result, privwarning, 22);
        } else {
            int file = hash(filename);
            getfile(file, result);
        }
        write(shm->filepipe[WR], result, 1024);
    } while (1); 


};

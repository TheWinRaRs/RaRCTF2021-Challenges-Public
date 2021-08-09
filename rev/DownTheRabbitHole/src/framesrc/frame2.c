#include "syscalls.h"
#include "puts.h"
#include "memcmp.h"
#include "hexlify.h"
#include "memset.h"
#include "aes.h"
#include "../loader.h"
#include "../flag1.h"

// Attempt to decrypt flag using hash

volatile void entry() {

	mbedtls_aes_context ctx;
	char fail[] = "Not quite!";

	struct sharedmem *shm = (struct sharedmem *)SHMBASE;
	unsigned char hash[16];
	read(shm->hashpipe[RD], hash, 16);
	char hex[33];
	char real[32] = KEY1;

	hexlify(hash, 16, hex);
	if (memcmp(hex, real, 32) != 0 ) {
		puts(fail);
		return;
	}
	char flag[48];
	char iv[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	mbedtls_aes_init( &ctx );
	mbedtls_aes_setkey_dec( &ctx, hash, 128 );
	mbedtls_aes_crypt_cbc( &ctx,
							MBEDTLS_AES_DECRYPT,
							32,
							iv,
							flagenc,
							flag);
	write(1, flag, 32);
}

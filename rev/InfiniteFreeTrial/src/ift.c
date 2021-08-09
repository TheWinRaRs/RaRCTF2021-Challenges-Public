#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flag.h"

char crcout[NCHUNKS] = {0};
char xorout[NCHUNKS * CHUNKSZ * 2] = {0};
int do_crc_check(char* buf);
int do_xor_check(char* buf);

uint8_t crc8(const void* vptr, int len) {
  const uint8_t *data = vptr;
  unsigned crc = 0;
  int i, j;
  for (j = len; j; j--, data++) {
    crc ^= (*data << 8);
    for(i = 8; i; i--) {
      if (crc & 0x8000)
        crc ^= (0x1070 << 3);
      crc <<= 1;
    }
  }
  uint8_t result = (uint8_t)(crc >> 8);
  return (uint8_t)(crc >> 8);
}

void xor_block(uint8_t* b1, uint8_t* b2, uint8_t* target, int size) {
    for (int i = 0; i < size; i++)
        target[i] = b1[i] ^ b2[i];
}

/*
int main() {
    char* buf = malloc(0x64);
    fgets(buf, 0x64, stdin);
    if (!do_crc_check(buf)) {
        puts("Invalid Serial Key");
        exit(-1);
    }

    if (!do_xor_check(buf)) {
        puts("Invalid Serial Key");
        exit(-1);
    }

    puts("Application Registered");
}
*/

int do_crc_check(char* buf) {
    for (int i = 0; i < NCHUNKS; i ++) {
        uint8_t res = crc8(buf + (i * CHUNKSZ), CHUNKSZ);
        crcout[i] = crccheck[res];
    }
    if (memcmp(crcout, "w1nR4rs", sizeof(crcout)) == 0) return 1;
    return 0;
}

int do_xor_check(char* buf) {
    for (int i = 0; i < NCHUNKS-1; i++) {
        xor_block(&buf[i*CHUNKSZ], &buf[(i+1)*CHUNKSZ], &xorout[i*CHUNKSZ], CHUNKSZ);
    }
    if (memcmp(xorout, xorcheck, sizeof(xorcheck)) == 0) return 1;
    return 0;
}

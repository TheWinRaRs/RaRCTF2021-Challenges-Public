#if !defined(_HEXLIFY_H)
#define _HEXLIFY_H

#include "itoa.h"

/*
  in: must be of at size of least len
  out: must be of size at least len*2 + 1 (for null byte)
*/
char* hexlify(const unsigned char* in,
             unsigned int len,
             char* out) {
  for (unsigned int i = 0; i < len; i++) {
    itoa(in[i], out + i*2, 3, 16, 2, '0');
  }
  return out;
}

#endif

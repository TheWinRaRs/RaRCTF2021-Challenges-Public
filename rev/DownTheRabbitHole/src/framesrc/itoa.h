#if !defined(_ITOA_H)
#define _ITOA_H

#include "strrev.h"

int itoa(int num, char* str, int len, int base, int width, char pad) {
  int sum = num;
  int i = 0;
  int digit;
  if (len == 0)
    return -1;
  if (width >= len) {
    return -1;
  }
  do {
    digit = sum % base;
    if (digit < 0xA)
      str[i++] = '0' + digit;
    else
      str[i++] = 'a' + digit - 0xA;
    sum /= base;
  } while (sum && (i < (len - 1)));
  if (i == (len - 1) && sum)
    return -1;
  for(; i < width; i++) {
    str[i] = pad;
  }
  str[i] = '\0';
  strrev(str);
  return 0;
}

#endif

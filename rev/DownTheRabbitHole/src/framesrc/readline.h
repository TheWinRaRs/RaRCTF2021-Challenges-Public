#if !defined(_READLINE_H)
#define _READLINE_H

#include "syscalls.h"

/*
 * reads a null terminated string
 * reads bytes from stdin until a newline, error or end of buffer
 */
size_t readline(char* buf, size_t len) {
  size_t read_len = 0;
  volatile char* b = (volatile char*)buf;
  if (len == 0) {
    return 0;
  }
  while (len - 1) {
    b[read_len] = 0;
    ssize_t i = read(stdin, (char*)b + read_len, 1);
    if (i <= 0) {
      break;
    }
    read_len++;
    len--;

    if (b[read_len - 1] == '\n') {
      break;
    }
  }
  b[read_len] = 0;
  return read_len;
}

#endif

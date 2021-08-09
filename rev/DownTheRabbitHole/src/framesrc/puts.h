#if !defined(_PUTS_H)
#define _PUTS_H

#include "strlen.h"
#include "syscalls.h"

int puts(const char*s) {
  int out = write(stdout, s, strlen(s));
  return out;
    // return (out + write(stdout, "\n", 1));
}

#endif

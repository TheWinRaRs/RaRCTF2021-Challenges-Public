#if !defined(_STRREV_H)
#define _STRREV_H

#include "strlen.h"

char* strrev(char *str) {
  int i;
  int j;
  char a;
  size_t len = strlen(str);
  for (i = 0, j = len - 1; i < j; i++, j--) {
    a = str[i];
    str[i] = str[j];
    str[j] = a;
  }
  return str;
}

#endif

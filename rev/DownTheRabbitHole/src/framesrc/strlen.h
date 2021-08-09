#if !defined(_STRLEN_H)
#define _STRLEN_H

#include <stddef.h>

size_t strlen(const char* s) {
  const char* p = s;
  while (*(s++) != '\0') {}
  return s - p - 1;
}

#endif

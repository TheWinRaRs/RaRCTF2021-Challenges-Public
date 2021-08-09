#if !defined(_MEMCMP_H)
#define _MEMCMP_H

/* Nonzero if either X or Y is not aligned on a "long" boundary.  */
#define UNALIGNED(X, Y) \
  (((long)X & (sizeof (long) - 1)) | ((long)Y & (sizeof (long) - 1)))

/* How many bytes are copied each iteration of the word copy loop.  */
#define LBLOCKSIZE (sizeof (long))

/* Threshhold for punting to the byte copier.  */
#define TOO_SMALL(LEN)  ((LEN) < LBLOCKSIZE)

int memcmp(const void *m1, const void *m2, size_t n) {
#if defined(PREFER_SIZE_OVER_SPEED) || defined(__OPTIMIZE_SIZE__)
  unsigned char *s1 = (unsigned char *) m1;
  unsigned char *s2 = (unsigned char *) m2;

  while (n--) {
    if (*s1 != *s2) {
      return *s1 - *s2;
    }
    s1++;
    s2++;
  }
  return 0;
#else
  unsigned char *s1 = (unsigned char *) m1;
  unsigned char *s2 = (unsigned char *) m2;
  unsigned long *a1;
  unsigned long *a2;

  /* If the size is too small, or either pointer is unaligned,
     then we punt to the byte compare loop.  Hopefully this will
     not turn up in inner loops.  */
  if (!TOO_SMALL(n) && !UNALIGNED(s1,s2)) {
    /* Otherwise, load and compare the blocks of memory one
       word at a time.  */
    a1 = (unsigned long*) s1;
    a2 = (unsigned long*) s2;
    while (n >= LBLOCKSIZE) {
      if (*a1 != *a2)
        break;
      a1++;
      a2++;
      n -= LBLOCKSIZE;
    }

    /* check m mod LBLOCKSIZE remaining characters */

    s1 = (unsigned char*)a1;
    s2 = (unsigned char*)a2;
  }

  while (n--) {
    if (*s1 != *s2)
      return *s1 - *s2;
    s1++;
    s2++;
  }

  return 0;
#endif /* not PREFER_SIZE_OVER_SPEED */
}

#undef UNALIGNED
#undef LBLOCKSIZE
#undef TOO_SMALL

#endif

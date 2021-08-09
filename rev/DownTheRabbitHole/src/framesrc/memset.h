#if !defined(_MEMSET_H)
#define _MEMSET_H

#define LBLOCKSIZE (sizeof(long))
#define UNALIGNED(X)   ((long)X & (LBLOCKSIZE - 1))
#define TOO_SMALL(LEN) ((LEN) < LBLOCKSIZE)

void *memset(void *m, int c, size_t n) {
  char *s = (char *) m;
#if !defined(PREFER_SIZE_OVER_SPEED) && !defined(__OPTIMIZE_SIZE__)
  int i;
  unsigned long buffer;
  unsigned long *aligned_addr;
  unsigned int d = c & 0xff;    /* To avoid sign extension, copy C to an
                                   unsigned variable.  */

  while (UNALIGNED (s)) {
    if (n--)
      *s++ = (char) c;
    else
      return m;
  }

  if (!TOO_SMALL (n)) {
    /* If we get this far, we know that n is large and s is word-aligned. */
    aligned_addr = (unsigned long *) s;

    /* Store D into each char sized location in BUFFER so that
       we can set large blocks quickly.  */
    buffer = (d << 8) | d;
    buffer |= (buffer << 16);
    for (i = 32; i < LBLOCKSIZE * 8; i <<= 1)
      buffer = (buffer << i) | buffer;

    /* Unroll the loop.  */
    while (n >= LBLOCKSIZE*4) {
      *aligned_addr++ = buffer;
      *aligned_addr++ = buffer;
      *aligned_addr++ = buffer;
      *aligned_addr++ = buffer;
      n -= 4*LBLOCKSIZE;
    }

    while (n >= LBLOCKSIZE)
      {
        *aligned_addr++ = buffer;
        n -= LBLOCKSIZE;
      }
    /* Pick up the remainder with a bytewise loop.  */
    s = (char*)aligned_addr;
  }

#endif /* not PREFER_SIZE_OVER_SPEED */

  while (n--)
    *s++ = (char) c;

  return m;
}

#undef LBLOCKSIZE
#undef UNALIGNED
#undef TOO_SMALL

#endif

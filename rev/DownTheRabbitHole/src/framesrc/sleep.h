#if !defined(_SLEEP_H)
#define _SLEEP_H

#include "syscalls.h"

void sleep(unsigned int duration_in_ms) {
  struct timespec ts, ts2;
  // volatile here prevents the below equations being optimised out
  *((volatile __kernel_time_t*)&ts.tv_sec) = duration_in_ms / 1000;
  *((volatile long*)&ts.tv_nsec) = (duration_in_ms % 1000) * 1000000;
  if (nanosleep(&ts, &ts2) == -1) {
    ts = ts2;
    nanosleep(&ts, &ts2);
  }
}

#endif

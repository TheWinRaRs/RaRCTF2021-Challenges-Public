#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <stddef.h>
#include <bits/types/siginfo_t.h>
#include <bits/types.h>
#include <linux/wait.h>
#include <linux/fcntl.h>
#include <linux/ptrace.h>
#include <linux/prctl.h>
#include <asm/signal.h>

/* define these here instead of include sys/types.h */
typedef __ssize_t ssize_t;
typedef __id_t id_t;

#define SYSCALL static inline __attribute__((always_inline))

#define DebugBreak() asm("int $3")

SYSCALL
int write(unsigned int fd,
          const char *buf,
          size_t count) {
  int register len;
  asm volatile (
       "mov $1, %%rax\n"
       "mov %1, %%edi\n"
       "mov %2, %%rsi\n"
       "mov %3, %%rdx\n"
       "syscall\n"
       "mov %%eax, %0\n"
       : "=r" (len)
       : "r" (fd), "r" (buf), "r" (count)
       : "rax", "rdi", "rsi", "rdx", "rcx", "r11"
       );
  return len;
}

SYSCALL
ssize_t read(unsigned int fd,
             char *buf,
             size_t count) {
  ssize_t register len;
  asm volatile (
       "mov $0, %%rax\n"
       "mov %1, %%edi\n"
       "mov %2, %%rsi\n"
       "mov %3, %%rdx\n"
       "syscall\n"
       "mov %%rax, %0\n"
       : "=r" (len)
       : "r" (fd), "r" (buf), "r" (count)
       : "rax", "rdi", "rsi", "rdx", "rcx", "r11"
       );
  return len;
}

SYSCALL
int pipe(int pipefd[2]) {
  int register result;
  asm volatile (
       "mov $22, %%rax\n"
       "mov %1, %%rdi\n"
       "syscall\n"
       "mov %%eax, %0\n"
       : "=r" (result)
       : "r" (pipefd)
       : "rax", "rdi", "rcx", "r11"
       );
  return result;
}

SYSCALL
void exit(int code) {
  asm volatile (
       "mov $60, %%rax\n"
       "mov %0, %%edi\n"
       "syscall\n"
       :
       : "r" (code)
       : "rax", "edi", "rcx", "r11"
       );
  while (1) {};
}

SYSCALL
int nanosleep(const struct timespec *req, struct timespec *rem) {
  int register result;
  asm volatile (
       "mov $35, %%rax\n"
       "mov %1, %%rdi\n"
       "mov %2, %%rsi\n"
       "syscall\n"
       "mov %%eax, %0\n"
       : "=r" (result)
       : "r" (req), "r" (rem)
       : "rax", "rdi", "rsi", "rcx", "r11"
       );
  return result;
}

SYSCALL
int openat(int dirfd,
           const char *pathname,
           int flags,
           int mode) {
  int register result;
  asm volatile (
       "mov $257, %%rax\n"
       "mov %1, %%edi\n"
       "mov %2, %%rsi\n"
       "mov %3, %%edx\n"
       "mov %4, %%r10d\n"
       "syscall\n"
       "mov %%eax, %0\n"
       : "=r" (result)
       : "r" (dirfd), "r" (pathname), "r" (flags), "r" (mode)
       : "rax", "edi", "rsi", "edx", "r10", "rcx", "r11"
       );
  return result;
}

SYSCALL
int close(int fd) {
  int register result;
  asm volatile (
       "mov $3, %%rax\n"
       "mov %0, %%edi\n"
       "syscall\n"
       "mov %%eax, %0\n"
       : "=r" (result)
       : "r" (fd)
       : "rax", "edi", "rcx", "r11"
       );
  return result;
}

#define stdin 0
#define stdout 1

#endif

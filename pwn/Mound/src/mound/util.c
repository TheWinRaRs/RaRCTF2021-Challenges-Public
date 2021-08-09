#include "mound.h"
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
unsigned long rand64bit(){
    unsigned long output;
    *(int *)&output = rand();
    *((int *)&output + 1) = rand();
    return output;
}
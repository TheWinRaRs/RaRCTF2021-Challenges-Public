#include <jni.h>
#include <android/log.h>
#include <string>
#include <algorithm>
#include <vector>
#include <dlfcn.h>
#include <sys/mman.h>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "aes.hpp"
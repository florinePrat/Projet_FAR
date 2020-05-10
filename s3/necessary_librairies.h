#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <regex.h>
#include <dirent.h>
#include <limits.h>
#include <semaphore.h>

#define MSG 200
#define NB_MAX_USERS 80
#define NB_MAX_FILES 100
#define _POSIX_SOURCE 1

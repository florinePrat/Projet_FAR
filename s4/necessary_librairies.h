#include <stdio.h>
#include <stdlib.h>
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
#define NB_MAX_CHANNEL 20
#define NAME_LENGTH 30
#define DESCRIPTION_LENGTH 70
#define PSEUDO_LENGTH 20

/**
  Parameter : the returned value of the pthread_create method

  exit the program if pthread_create failed
**/
void check_thread(int);

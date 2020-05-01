#ifndef PROJECT_2_AUXILIARY_H
#define PROJECT_2_AUXILIARY_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

extern struct timespec start;

void regist_message(int i, pid_t pid, pid_t tid, int dur, int pl, char *oper);

struct Message {
  int i;
  pid_t pid;
  pthread_t tid;
  int dur;
  int pl;
};

#endif

#ifndef PROJECT_2_AUXILIARY_H
#define PROJECT_2_AUXILIARY_H

#include <sys/times.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct Message {
  int i;
  pid_t pid;
  pthread_t tid;
  int dur;
  int pl;
} message_t;

void register_message(message_t * message, char * oper);

double get_exec_time(clock_t start);

#endif

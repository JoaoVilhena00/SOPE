#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <time.h>
#include "auxiliary.h"

#define NUMTHRDS 5
#define MAXUSETIME 300
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
int fd, i = 0;
struct timespec start;

void print_usage() {
  printf("\nArguments not valid!");
  printf("\nUsage: U1 <-t nsecs> fifoname\n");
  exit(1);
}

void print_argv(int argc, char *argv[]) {
  printf("\n----- PRINT ARGV -----\n");
  for (int i = 0; i < argc; i++) {
    printf("argv[%d]: %s\n", i, argv[i]);
  }
  printf("----------------------\n");
}

void print_options(int argc, char *options[], int nsecs, char *fifoname) {
  if(nsecs != -1) {
    argc -= 1;
  }

  printf("\n----- PRINT OPTIONS -----\n");
  for (int i = 1; i < argc; i++) {
    printf("options[%d]: %s\n", i-1, options[i-1]);
  }
  if(nsecs != -1) {
    printf("nsecs = %d\n", nsecs);
  }
  printf("fifoname: %s\n", fifoname);
  printf("-------------------------\n");

}

int get_options(int argc, char *argv[], char *options[], int *nsecs, char *fifoname) {
  int aux = 0;
  int j = 0;

  if(argc > 4 || argc < 2)
    print_usage();

  for(int i = 1; i < argc; i++) {
    strcpy(options[j], argv[i]);
    if(strcmp(argv[i], "-t") == 0){
      i++;
      if(i >= argc)
        print_usage();
      sscanf(argv[i], "%d", nsecs);

    } else {
      strcpy(fifoname, argv[i]);
      aux++;
    }
    j++;
  }

  if (aux == argc-1 && argc == 3)
    print_usage();

  return 0;
}



void createPrivateFIFO(pthread_t tid) {

  char fifoname[50], tidStr[20], pidStr[6];
  pid_t pid = getpid();
  

  sprintf(tidStr, "%ld", tid);
  sprintf(pidStr, "%d", pid);
  strcpy(fifoname, "/tmp/");
  strcat(fifoname, pidStr);
  strcat(fifoname, ".");
  strcat(fifoname, tidStr);

  //printf("FifoName: %s\n", fifoname);

  if(mkfifo(fifoname, O_RDONLY) < 0) {
    perror("FIFO error");
  }
}

void sendOrder(char *fifoname, int dur, pthread_t tid) {

  char Message[70]; //, iStr[5], pidStr[10], tidStr[15], durStr[10];

  sprintf(Message, "%d", dur);
  printf("My time is: %s\n", Message);
  

  /*
  pthread_mutex_lock(&mut);
  i++;
  strcpy(Message, "[");
  sprintf(iStr, "%d", i);
  strcat(Message, iStr);
  strcat(Message, ",");

  sprintf(pidStr, "%d", getpid());
  strcat(Message, pidStr);
  strcat(Message, ",");

  sprintf(tidStr, "%ld", tid);
  strcat(Message, tidStr);
  strcat(Message, ",");

  sprintf(durStr, "%d", dur);
  strcat(Message, durStr);
  strcat(Message, ",");

  strcat(Message, "-1");
  strcat(Message, "]");
  pthread_mutex_unlock(&mut);

  printf("%s\n", Message);
  */
  
  write(fd, Message, sizeof(Message));
}


void *client(void *arg) {

  int usingTime = rand() % MAXUSETIME;
  pthread_t tid = pthread_self();

  
  sendOrder((char *) arg, usingTime, tid);
  createPrivateFIFO(tid);
  
  
  pthread_exit(NULL);
}

void create_threads(int nsecs, char *fifoname) {

  pthread_t tid;

  pthread_create(&tid, NULL, client, (void*) fifoname);
  sleep(0.005);

}

void openFIFOforWriting(char *fifoname) {

  do {
    if((fd = open("/tmp/door1", O_WRONLY)) == -1){
      perror("File Error");
    }
    if(fd == -1)
      sleep(1);

  }while(fd == -1);
}


int main(int argc, char *argv[]) {
  char *options[8];
  char fifoname[15];
  int nsecs = -1;
  srand(time(NULL));
  clock_t start_t, end_t, total_t;


  for (int i = 0; i < 8; i++) {
    *(options + i) = (char *) malloc(15 * sizeof(char));
  }

  get_options(argc, argv, options, &nsecs, fifoname);
  clock_gettime(CLOCK_MONOTONIC_RAW, &start);

  print_options(argc, options, nsecs, fifoname);

  openFIFOforWriting(fifoname);

  //clock_gettime(CLOCK_MONOTONIC_RAW, &start);


  start_t=clock();
  end_t=clock();
  total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;


  while(total_t < nsecs){
    create_threads(nsecs, fifoname);
    end_t=clock();
    total_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
  }

  close(fd);
  pthread_exit(0);
}

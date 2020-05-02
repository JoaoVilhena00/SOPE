#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <time.h>
#include "auxiliary.h"

#define NUMTHRDS 5
#define MAXUSETIME 300
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
int fd, i = 0;
#define BILLION  1000000000.0

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

int createPrivateFIFO(char *fifoname) {

  if(mkfifo(fifoname, 0660) < 0) {
    perror("FIFO error");
  } else {
    printf("Created FIFO - %s\n", fifoname);
  }

  return open(fifoname, O_RDONLY);
}

void sendOrder(int seq_i, int dur, pthread_t tid) {

  struct Message message;

  message.i = seq_i;
  message.pid = getpid();
  message.tid = tid;
  message.dur = dur;
  message.pl = -1;

  printf("Sent Order - i: %d - dur: %d\n", message.i, message.dur);

  write(fd, &message, sizeof(message));
}

void getAnswer(char * fifoname, int int_answer) {
  struct Message answer;
  int n;

  n = read(int_answer, &answer, sizeof(answer));
  usleep(100000);

  if (n > 0) {
    printf("Received Answer - i: %d - pl: %d\n", answer.i, answer.pl);
  }
}

void *client(void *arg) {

  int usingTime = rand() % MAXUSETIME;
  int seq_i;
  pthread_t tid;
  int int_answer;

  char fifoname[64];
  pid_t pid;

  pid = getpid();

  seq_i = *((int*) arg);
  tid = syscall(SYS_gettid);

  sendOrder(seq_i, usingTime, tid);

  sprintf(fifoname, "/tmp/%d.%ld", pid, tid);
  int_answer = createPrivateFIFO(fifoname);

  getAnswer(fifoname, int_answer);

  close(int_answer);
  unlink(fifoname);

  pthread_exit(NULL);
}

void create_threads(int nsecs, char *fifoname, int *seq_i) {
  pthread_t tid;

  pthread_create(&tid, NULL, client, (void*) seq_i);
  pthread_join(tid, NULL);
  usleep(100000);
}

void openFIFOforWriting(char *fifoname) {

  do {
    if((fd = open(fifoname, O_WRONLY)) == -1){
      perror("File Error");
    }
    if(fd == -1)
      sleep(1);

  }while(fd == -1);
}


int main(int argc, char *argv[]) {
  char *options[8];
  char name[64];
  char fifoname[64];
  int nsecs = -1, seq_i = 1;

  struct timespec start;
  struct timespec end;
  double accum;

  for (int i = 0; i < 8; i++) {
    *(options + i) = (char *) malloc(15 * sizeof(char));
  }

  get_options(argc, argv, options, &nsecs, name);

  print_options(argc, options, nsecs, name);

  strcpy(fifoname, "/tmp/");
  strcat(fifoname, name);

  openFIFOforWriting(fifoname);

  if(clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }

  clock_gettime(CLOCK_REALTIME, &end);
  accum = ( end.tv_sec - start.tv_sec )
          + ( end.tv_nsec - start.tv_nsec )
            / BILLION;

  while(accum < nsecs){
    create_threads(nsecs, fifoname, &seq_i);
    clock_gettime(CLOCK_REALTIME, &end);
    accum = ( end.tv_sec - start.tv_sec )
            + ( end.tv_nsec - start.tv_nsec )
              / BILLION;
    seq_i++;
  }

  close(fd);
  pthread_exit(0);
}

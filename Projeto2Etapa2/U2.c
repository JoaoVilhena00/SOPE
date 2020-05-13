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
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "auxiliary.h"

#define NUMTHRDS 5
#define MAXUSETIME 300
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
int fd, i = 0;
int full, empty;
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
  }

  return open(fifoname, O_RDONLY);
}

void sendOrder(struct Message message) {
  write(fd, &message, sizeof(message));

  regist_message(message.i, message.pid, message.tid, message.dur, message.pl, "IWANT");
}

struct Message getAnswer(char * fifoname, int int_answer) {
  struct Message answer;
  int n;

  n = read(int_answer, &answer, sizeof(answer));
  usleep(100000);

  if (n > 0) {
    if (answer.pl != -1)
      regist_message(answer.i, answer.pid, answer.tid, answer.dur, answer.pl, "IAMIN");
  } else {
    answer.i = -1;
  }

  return answer;
}

void *client(void *arg) {

  int dur = rand() % MAXUSETIME;
  int seq_i;
  pthread_t tid;
  int int_answer;
   pthread_detach(tid = pthread_self());

char fifoname[64];
  pid_t pid;

  pid = getpid();

  seq_i = *((int*) arg);
  
 tid = pthread_self();

  struct Message request;
  struct Message answer;

  request.i = seq_i;
  request.pid = getpid();
  request.tid = tid;
  request.dur = dur;
  request.pl = -1;

  sendOrder(request);

  sprintf(fifoname, "/tmp/%d.%ld", pid, tid);
  int_answer = createPrivateFIFO(fifoname);

  answer = getAnswer(fifoname, int_answer);

  if(answer.i == -1) {
    regist_message(request.i, request.pid, request.tid, request.dur, request.pl, "FAILD");
  }

  if(answer.pl == -1) {
    regist_message(request.i, request.pid, request.tid, request.dur, request.pl, "CLOSD");
  }

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
  int *pt, shmid;

  for (int i = 0; i < 8; i++) {
    *(options + i) = (char *) malloc(15 * sizeof(char));
  }

  get_options(argc, argv, options, &nsecs, name);

  if((shmid = shmget(IPC_PRIVATE, 1024, 0666)) == -1) {
    perror("SHMGET Error");
    exit(1);
  }
  
  pt = (int *) shmat(shmid, NULL, 0);
  if((int *) pt == (int *)-1) {
    perror("SHMAT  Error");
    exit(1);
  }

  empty = pt[0];
  full = pt[1];
    
  printf("%d %d\n", empty, full);

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
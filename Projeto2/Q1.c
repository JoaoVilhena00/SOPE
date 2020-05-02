#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "auxiliary.h"

#define NUMTHRDS 10000
#define BILLION  1000000000.0
int fd;
int place_i = 1;
int * places;
int nsecs = -1, nplaces = -1, nthreads = -1;
struct timespec start;
struct timespec end;
double accum;
int opened = 0;

void print_argv(int argc, char *argv[]) {
  printf("\n----- PRINT ARGV -----\n");
  for (int i = 0; i < argc; i++) {
    printf("argv[%d]: %s\n", i, argv[i]);
  }
  printf("----------------------\n");
}

void print_usage() {
  printf("\nArguments not valid!");
  printf("\nUsage: Q1 <-t nsecs> [-l nplaces] [-n nthreads] fifoname\n");
  exit(1);
}

void print_options(int argc, char *options[], char *fifoname) {
    if(nsecs != -1 || nplaces != -1 || nthreads != -1) {
        argc -= 1;
    }
    printf("\n----- PRINT OPTIONS -----\n");
    for (int i = 1; i < argc; i++) {
        printf("options[%d]: %s\n", i-1, options[i-1]);
    }
    if(nsecs != -1) {
        printf("nsecs = %d\n", nsecs);
    }
    if(nplaces != -1) {
        printf("nplaces = %d\n", nplaces);
    }
    if(nthreads != -1) {
        printf("nthreads = %d\n", nthreads);
    }
    printf("fifoname: %s\n", fifoname);
    printf("-------------------------\n");
}

void get_options(int argc, char *argv[], char *options[], char *fifoname) {

    int j=0;

    if(argc > 8) {
        print_usage();
    }

    for(int i=1; i<argc; i++) {
        strcpy(options[j], argv[i]);
        if(strcmp(argv[i], "-t") == 0){
            i++;
            if(i >= argc)
               print_usage();
            sscanf(argv[i], "%d", &nsecs);
        }else if(strcmp(argv[i], "-l") == 0) {
            i++;
            if(i >= argc) {
                print_usage();
            }
            sscanf(argv[i], "%d", &nplaces);
        }else if(strcmp(argv[i], "-n") == 0) {
            i++;
            if(i >= argc) {
                print_usage();
            }
            sscanf(argv[i], "%d", &nthreads);
        } else {
            strcpy(fifoname, argv[i]);
        }
        j++;
    }
}

void print_places() {
  printf("\n----- PLACES -----\n");
  for(int i = 0; i < 5; i++) {
    printf("[%d]: %d\n", i, *(places+i));
  }
  printf("--------------------\n");
}

void *server(void *arg) {
    struct Message * request;
    struct Message answer;
    char fifo_answer[64];
    int int_answer;
    pid_t pid;
    pthread_t tid;

    pid = getpid();
    tid = syscall(SYS_gettid);

    request = (struct Message *) arg;
    regist_message(request->i, request->pid, request->tid, request->dur, request->pl, "RECVD");

    sprintf(fifo_answer, "/tmp/%d.%ld", request->pid, request->tid);
    int_answer = open(fifo_answer, O_WRONLY);

    if (int_answer >= 0) {
    } else {
      regist_message(request->i, request->pid, request->tid, request->dur, request->pl, "GAVUP");
    }

    answer.i = request->i;
    answer.pid = pid;
    answer.tid = tid;
    answer.dur = request->dur;

    if (opened == 0) {
      answer.pl = -1;
      write(int_answer, &answer, sizeof(answer));
      regist_message(answer.i, answer.pid, answer.tid, answer.dur, answer.pl, "2LATE");
      pthread_exit(NULL);
    }

    for(int i = 0; i < 5; i++) {
      if(*(places + i) == 0) {
        answer.pl = i + 1;
        *(places + i) = 1;
        break;
      } else {
        answer.pl = -1;
      }
    }

    clock_gettime(CLOCK_REALTIME, &end);
    accum = ( end.tv_sec - start.tv_sec )
            + ( end.tv_nsec - start.tv_nsec )
              / BILLION;

    if(answer.pl != -1 && (request->dur / 1000.0  + accum) <= nsecs) {
      regist_message(answer.i, answer.pid, answer.tid, answer.dur, answer.pl, "ENTER");
      usleep(request->dur * 1000);
      *(places + answer.pl - 1) = 0;
      regist_message(answer.i, answer.pid, answer.tid, answer.dur, answer.pl, "TIMUP");
    } else if(answer.pl != -1 && request->dur + accum > nsecs) {
      regist_message(answer.i, answer.pid, answer.tid, answer.dur, answer.pl, "2LATE");
    }

    write(int_answer, &answer, sizeof(answer));

    close(int_answer);

    pthread_exit(NULL);
}

void createPublicFIFO(char *fifoname) {

    unlink(fifoname);

    if(mkfifo(fifoname, 0660) < 0) {
        perror("FIFO Error");
        exit(1);
    }

    if((fd = open(fifoname, O_RDONLY)) < 0){
        perror("File Error");
        exit(2);
    }
}

int main(int argc, char *argv[]) {
    char *options[8];
    char name[64];
    char fifoname[64];
    print_argv(argc, argv);

    opened = 1;

    for (int i = 0; i < 8; i++) {
        *(options + i) = (char *) malloc(15 * sizeof(char));
    }

    get_options(argc, argv, options, name);

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    print_options(argc, options, name);

    strcpy(fifoname, "/tmp/");
    strcat(fifoname, name);

    createPublicFIFO(fifoname);

    places = (int *) calloc(20, sizeof(int));

    if(clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
      }

    clock_gettime(CLOCK_REALTIME, &end);
    accum = ( end.tv_sec - start.tv_sec )
            + ( end.tv_nsec - start.tv_nsec )
              / BILLION;

    while(accum < nsecs){
      pthread_t tid;
      int nr = 0;
      struct Message message;

      while (nr <= 0 && accum < nsecs) {
        nr = read(fd, &message, sizeof(message));
        usleep(100000);
        clock_gettime(CLOCK_REALTIME, &end);
        accum = ( end.tv_sec - start.tv_sec )
                + ( end.tv_nsec - start.tv_nsec )
                  / BILLION;
      }

      pthread_create(&tid, NULL, server, &message);
      pthread_join(tid,NULL);

      clock_gettime(CLOCK_REALTIME, &end);
      accum = ( end.tv_sec - start.tv_sec )
              + ( end.tv_nsec - start.tv_nsec )
                / BILLION;
    }

    opened = 0;

    pthread_t tid;
    int nr = 0;
    struct Message message;

    do {
      nr = read(fd, &message, sizeof(message));
      usleep(100000);

      if(nr > 0) {
        pthread_create(&tid, NULL, server, &message);
        pthread_join(tid,NULL);
      }
    } while (nr > 0);

    unlink(fifoname);

    close(fd);

    free(places);

    return 0;
}

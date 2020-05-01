#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define NUMTHRDS 10000
#define BILLION  1000000000.0
int fd, fd2;

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

void print_options(int argc, char *options[], int nsecs, int nplaces, int nthreads, char *fifoname) {
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

void get_options(int argc, char *argv[], char *options[], int *nsecs, int *nplaces, int *nthreads, char *fifoname) {

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
            sscanf(argv[i], "%d", nsecs);
        }else if(strcmp(argv[i], "-l") == 0) {
            i++;
            if(i >= argc) {
                print_usage();
            }
            sscanf(argv[i], "%d", nplaces);
        }else if(strcmp(argv[i], "-n") == 0) {
            i++;
            if(i >= argc) {
                print_usage();
            }
            sscanf(argv[i], "%d", nthreads);
        } else {
            strcpy(fifoname, argv[i]);
        }
        j++;
    }
}

void writeAnswer() {




}



void *server(void *arg) {

    printf("==>%s\n", (char *) arg);

    do {
        fd2 = open();
    }while(fd2 == -1);




    pthread_exit(NULL);
}

void createPublicFIFO(char *fifoname) {

    unlink("/tmp/door1");


    if(mkfifo("/tmp/door1", 0660) < 0) {
        perror("FIFO Error");
        exit(1);
    }

    if((fd = open("/tmp/door1", O_RDONLY)) < 0){
        perror("File Error");
        exit(2);
    }
}

int main(int argc, char *argv[]) {
    char *options[8];
    char fifoname[15];
    int nsecs = -1, nplaces = -1, nthreads = -1;
    print_argv(argc, argv);

    struct timespec start;
    struct timespec end;
    double accum;

    for (int i = 0; i < 8; i++) {
        *(options + i) = (char *) malloc(15 * sizeof(char));
    }

    get_options(argc, argv, options, &nsecs, &nplaces, &nthreads, fifoname);

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    print_options(argc, options, nsecs, nplaces, nthreads, fifoname);

    createPublicFIFO(fifoname);

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
      char message[40];

      while (nr <= 0 && accum < nsecs) {
        nr = read(fd, message, sizeof(message));
        usleep(100000);
      }

      pthread_create(&tid, NULL, server, (void*) message);
      pthread_join(tid,NULL);

      clock_gettime(CLOCK_REALTIME, &end);
      accum = ( end.tv_sec - start.tv_sec )
              + ( end.tv_nsec - start.tv_nsec )
                / BILLION;
    }


    unlink("/tmp/door1");

    close(fd);

    return 0;
}

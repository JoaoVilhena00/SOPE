#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define NUMTHRDS 5
int fd;

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

void *server(void *arg) {

    int nr;
    char message[15];
    static int i = -1;
    pthread_t selftid = pthread_self();

    
    do {  
        nr = read(fd, message, 10);
        i++;
    }while(nr>0 && message[i] != '\0');
    
    printf("====>%s\n",message);//alguns threads nao recebem nada

    pthread_exit(NULL);
}

void create_threads(int nsecs, char *fifoname) {

    pthread_t tid[NUMTHRDS];

    for(int i=0; i<NUMTHRDS; i++) {
        pthread_create(&tid[i], NULL, server, (void*) fifoname);
        sleep(0.005);
    }

    for(int j=0; j<NUMTHRDS; j++) {
        pthread_join(tid[j],NULL);
        printf("I m thread %ld and i just finished!\n", tid[j]);
    }
}

void createPublicFIFO(char *fifoname) {

    unlink("/tmp/door1");
    char message[5];

    if(mkfifo("/tmp/door1", 0660) < 0) {
        perror("FIFO Error");
        exit(1);
    }

    if((fd = open("/tmp/door1", O_RDONLY)) < 0){ //Ele fica preso aqui e o open esta estrano nao consigo resolver
        perror("File Error");
        exit(2);
    }
}

int main(int argc, char *argv[]) {
    char *options[8];
    char fifoname[15];
    int nsecs = -1, nplaces = -1, nthreads = -1;

    print_argv(argc, argv);

    for (int i = 0; i < 8; i++) {
        *(options + i) = (char *) malloc(15 * sizeof(char));
    }

    get_options(argc, argv, options, &nsecs, &nplaces, &nthreads, fifoname);

    print_options(argc, options, nsecs, nplaces, nthreads, fifoname);

    createPublicFIFO(fifoname);

    create_threads(nsecs, fifoname);

    unlink("/tmp/door1");

    return 0;
}

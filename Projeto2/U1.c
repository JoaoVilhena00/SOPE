#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <time.h>

#define NUMTHRDS 5
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
int fd;

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

  if(argc > 4)
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

void sendOrder(char *fifoname, int usingTime) {

  char timeMessage[4];
  
  sprintf(timeMessage, "%d", usingTime);
  write(fd, timeMessage, strlen(timeMessage)+1);
}

void *client(void *arg) {
  
  pthread_t selftid = pthread_self();
  int usingTime = rand();

  //sendOrder((char *) arg, usingTime);

  pthread_exit(NULL);

}

void create_threads(int nsecs, char *fifoname) {
  
  pthread_t tid[NUMTHRDS];
  

  for(int i=0; i<NUMTHRDS; i++) {
    pthread_create(&tid[i], NULL, client, (void*) fifoname);
    sleep(0.005);
  }
  
  for(int j=0; j<NUMTHRDS; j++) {
    pthread_join(tid[j],NULL);
    printf("I m thread %ld and i just finished!\n", tid[j]);
  }
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
  char fifoname[15];
  int nsecs = -1;
  srand(time(NULL));
  
  for (int i = 0; i < 8; i++) {
    *(options + i) = (char *) malloc(15 * sizeof(char));
  }

  get_options(argc, argv, options, &nsecs, fifoname);

  print_options(argc, options, nsecs, fifoname);

  

  openFIFOforWriting(fifoname);

  

  create_threads(nsecs, fifoname);
  
  
  pthread_exit(0);
}

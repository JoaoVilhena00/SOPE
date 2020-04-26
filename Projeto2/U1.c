#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define NUMTHRDS 500

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

void *pedido(void *arg) {
  
  printf("I m executing....");


  pthread_exit(NULL);

}


void create_threads(int nsecs, char *fifoname) {
  
  pthread_t tid[NUMTHRDS];
  int j = 0;

  

  while((nsecs = sleep(nsecs)) > 0 || j >= 11) {
    tid[j] = j+1; 
    pthread_create(&tid[j], NULL, pedido, NULL);
    sleep(0.005);
    j++;
  }

  printf("****");

  for(int i=0; i<NUMTHRDS; i++) {
    pthread_join(tid[i],NULL);
    printf("I m thread %ld and i just finished!\n", tid[i]);
  }

  
}

int main(int argc, char *argv[]) {
  char *options[8];
  char fifoname[15];
  int nsecs = -1;
  

  //print_argv(argc, argv);

  for (int i = 0; i < 8; i++) {
    *(options + i) = (char *) malloc(15 * sizeof(char));
  }

  get_options(argc, argv, options, &nsecs, fifoname);

  

  print_options(argc, options, nsecs, fifoname);

  

  create_threads(nsecs, fifoname);

  
  pthread_exit(0);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Q2_aux.h"

void print_usage_Q2(){
  printf("Usage: Qn <-t nsecs> [-l nplaces] [-n nthreads] fifoname\n");
  exit(1);
}

int get_options_Q2(int argc, char *argv[], int *nsecs, char *fifoname, int *nplaces, int *nthreads){
  int i;

  if(argc < 4 || argc > 8)
    print_usage_Q2();

  if(!strcmp(argv[1], "-t")) {
    if(sscanf(argv[2], "%d", nsecs) != 1){
       print_usage_Q2();
    }
  } else {
    print_usage_Q2();
  }

  for (i = 3; i < argc; i++) {
    if(!strcmp(argv[i], "-l")) {
      i++;
      if(sscanf(argv[i], "%d", nplaces) != 1){
         print_usage_Q2();
      }
    } else if(!strcmp(argv[i], "-n")) {
      i++;
      if(sscanf(argv[i], "%d", nthreads) != 1){
         print_usage_Q2();
      }
    } else {
      strcpy(fifoname, argv[i]);
    }
  }

  return 0;
}

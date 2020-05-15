#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "U2_aux.h"

void print_usage_U2(){
  printf("Usage: Un <-t nsecs> fifoname\n");
  exit(1);
}

int get_options_U2(int argc, char *argv[], int *nsecs, char *fifoname){
  if(argc != 4)
    print_usage_U2();

  if(!strcmp(argv[1], "-t")){
    if(sscanf(argv[2], "%d", nsecs) != 1){
       print_usage_U2();
    }
  } else {
    print_usage_U2();
  }

  strcpy(fifoname, argv[3]);

  return 0;
}

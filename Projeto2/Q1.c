#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <pthread.h>
#include "auxiliary.h"

void get_options(int argc, char *argv[], char *options[], int *nsecs, int *nplaces, int *nthreads, char *fifoname) {

    int j=0;

    if(argc < 5) {
        //print error
    }

    for(int i=1; i<argc; i++) {
        strcpy(options[j], argv[i]);
        if(strcmp(argv[i], "-t") == 0){
            i++;
            if(i >= argc)
               //print error
            sscanf(argv[i], "%d", nsecs);
        }else if(strcmp(argv[i], "-l") == 0) {
            i++;
            if(i >= argc) {
                //print error
            }
            sscanf(argv[i], "%d", nplaces);
        }else if(strcmp(argv[i], "-l") == 0) {
            i++;
            if(i >= argc) {
                //print error
            }
            sscanf(argv[i], "%d", nthreads);
        } else {
            strcpy(fifoname, argv[i]);
        }
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

  

  return 0;
}
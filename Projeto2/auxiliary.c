#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "auxiliary.h"

void print_usage() {
  printf("\nArguments not valid!");
  printf("\nUsage: U1 <-t nsecs> fifoname\n");
  exit(1);
}
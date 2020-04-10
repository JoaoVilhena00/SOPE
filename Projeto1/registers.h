#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <wait.h>
#include <fcntl.h>
#include <signal.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include "auxiliary.h"

//each function is for a different type of action




void action_Exit(int status);

void action_create(int pid);

void action_sendPipe(long int size);

void action_recievePipe(long int size);

void action_entry(long int numberblocks,char* path);
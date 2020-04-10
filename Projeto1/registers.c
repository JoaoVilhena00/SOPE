
#include "registers.h"
#define BUFFER 512



//each function is for a different type of action


void action_Exit(int status) {
  char message[64];
  sprintf(message, "%.02f - %-8d - EXIT - %d\n", time_interval(), getpid(), status);
  write(logfile, message, strlen(message));
  exit(status);
}

void action_create(int pid) {
  char message[64];
  sprintf(message, "%.02f - %-8d - CREATE - ", time_interval(), getpid());
 
    for(int i=1;i<global_argc;i++){
    write(logfile,global_argv[i],sizeof(global_argv[1]));
    write(logfile," ",1);
}
  write(logfile, message, strlen(message));
}

void action_sendPipe(long int size) {
  char message[64];
  sprintf(message, "%.02f - %-8d - SEND_PIPE - %ld\n", time_interval(), getpid(), size);
  write(logfile, message, strlen(message));
}

void action_recievePipe(long int size) {
  char message[64];
  sprintf(message, "%.02f - %-8d - RECV_PIPE - %ld\n", time_interval(), getpid(), size);
  write(logfile, message, strlen(message));
}

void action_entry(long int numberblocks,char* path) {
  char* message;
  message = (char*) malloc (BUFFER * 2 * sizeof(char));
  sprintf(message, "%.02f - %-8d - ENTRY - %ld %s", time_interval(), getpid(),numberblocks, path);
  write(logfile, message, strlen(message));
  free(message);
}
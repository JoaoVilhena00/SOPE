#include "auxiliary.h"


void regist_message(int i, pid_t pid, pid_t tid, int dur, int pl, char *oper) {
    char* message;
    message = (char*) malloc (128 * sizeof(char));
    time_t t = time(NULL);

    sprintf(message, "%ld ; %d ; %d ; %d ; %d ; %d ; %s\n", t, i, pid, tid, dur, pl, oper);
    write(STDOUT_FILENO, message, strlen(message));
}

double time_interval(){
 struct timespec end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    return (end.tv_sec - start.tv_sec);
}
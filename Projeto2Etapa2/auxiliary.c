#include "auxiliary.h"

void regist_message(int i, pid_t pid,  pthread_t tid, int dur, int pl, char *oper) {
    char* message;
    message = (char*) malloc (128 * sizeof(char));
    time_t t = time(NULL);

    sprintf(message, "%ld ; %d ; %d ; %ld ; %d ; %d ; %s\n", t, i, pid, tid, dur, pl, oper);
    write(STDOUT_FILENO, message, strlen(message));
}

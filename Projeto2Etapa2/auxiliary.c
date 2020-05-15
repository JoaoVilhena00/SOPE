#include "auxiliary.h"

void register_message(message_t * message, char * oper) {
  char* m;
  time_t t;

  m = (char *) malloc(128 * sizeof(char));

  t = time(NULL);

  sprintf(m, "%ld ; %d ; %d ; %ld ; %d ; %d ; %s\n", t, message->i, message->pid, message->tid, message->dur, message->pl, oper);
  write(STDOUT_FILENO, m, strlen(m));

  free(m);
}

double get_exec_time(clock_t start) {
  struct tms t;
  clock_t end;
  long ticks;

  ticks = sysconf(_SC_CLK_TCK);

  end = times(&t);

  return (double)(end - start)/ticks;
}

#include <sys/times.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "U2_aux.h"
#include "auxiliary.h"

#define MAX 64
#define MAXUSETIME 300
int public_fd;

void open_public_FIFO(char *fifoname) {
  while((public_fd = open(fifoname, O_WRONLY)) == -1) {
    perror("Error Opening Public FIFO");
    sleep(1);
  }
}

char *create_private_FIFO(message_t *request) {
  char *fifoname;

  fifoname = (char *) malloc(MAX * sizeof(char));
  sprintf(fifoname, "/tmp/%d.%ld", request->pid, request->tid);
  mkfifo(fifoname, 0660);
  return fifoname;
}

int open_private_FIFO(char *fifoname) {
  return open(fifoname, O_RDONLY);
}

void delete_private_FIFO(char *fifoname, int private_fd) {
  close(private_fd);
  unlink(fifoname);
}

void sendRequest(message_t request) {
  if(write(public_fd, &request, sizeof(request)) != -1) {
    register_message(&request, "IWANT");
  }
}

int readAnswer(int private_fd) {
  message_t answer;

  while(read(private_fd, &answer, sizeof(answer)) <= 0){

  }

  if(answer.pl != -1)
    register_message(&answer, "ENTER");
  else
    register_message(&answer, "CLOSD");

  return 0;
}

void * client(void *arg) {
  pid_t pid;
  pthread_t tid;
  int dur, private_fd;
  message_t request;
  char *private_fifo_name;

  pid = getpid();
  tid = pthread_self();

  pthread_detach(tid);

  dur = rand() % MAXUSETIME;

  request.i = *((int*) arg);
  request.pid = pid;
  request.tid = tid;
  request.dur = dur;
  request.pl = -1;

  sendRequest(request);

  private_fifo_name = create_private_FIFO(&request);
  private_fd = open_private_FIFO(private_fifo_name);

  if(readAnswer(private_fd) != 0) {
    register_message(&request, "FAILD");
  }

  delete_private_FIFO(private_fifo_name, private_fd);

  return NULL;
}

int main(int argc, char *argv[]) {
  clock_t start;
  struct tms t;
  int nsecs, seq_i = 1;
  char *fifoname;

  fifoname = (char *) malloc(MAX * sizeof(char));
  get_options_U2(argc, argv, &nsecs, fifoname);

  open_public_FIFO(fifoname);

  start = times(&t); // Inicio da medicao do tempo

  while(get_exec_time(start) < (double) nsecs) {
    pthread_t tid;
    pthread_create(&tid, NULL, client, &seq_i);
    usleep(100000);
    seq_i++;
  }

  close(public_fd);

  free(fifoname);

  return 0;
}

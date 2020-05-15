#include <sys/times.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>

#include "Q2_aux.h"
#include "auxiliary.h"

#define MAX 64
clock_t start;
int nsecs = -1, nplaces = -1, nthreads = -1, public_fd, place_i, opened = 0;
static sem_t nthreadsactive, nplacesactive;
int *places;

void create_public_FIFO(char *fifoname) {
  mkfifo(fifoname, 0660);
}

int open_private_FIFO(message_t *request) {
  char *fifoname;

  fifoname = (char *) malloc(MAX * sizeof(char));
  sprintf(fifoname, "/tmp/%d.%ld", request->pid, request->tid);

  return open(fifoname, O_WRONLY);
}

void open_public_FIFO(char *fifoname) {
  while((public_fd = open(fifoname, O_RDONLY)) == -1) {
    perror("Error Opening Public FIFO");
    sleep(1);
  }
}

void sendAnswer(message_t answer, int private_fd) {
  if(answer.pl != -1) {
    if(write(private_fd, &answer, sizeof(answer)) != -1) {
      if(nplaces > 0) {
        places[answer.pl] = 1;
      }
      register_message(&answer, "ENTER");
    }
  } else {
    if(write(private_fd, &answer, sizeof(answer)) != -1) {
      register_message(&answer, "2LATE");
    }
  }
}

int getFreePlace() {
  int i;

  for(i = 0; i < nplaces; i++) {
    if (places[i] == 0)
      return i;
  }

  return -1;
}

void emptyPlace(int n) {
  places[n] = 0;

}

void *server(void *arg) {
  pthread_t tid;
  message_t *request;
  int private_fd;
  pid_t pid;
  message_t answer;

  pid = getpid();
  tid = pthread_self();

  pthread_detach(tid);

  request = (message_t *) arg;

  register_message(request, "RECVD");

  private_fd = open_private_FIFO(request);
  if(private_fd == -1) {
    register_message(request, "GAVUP");
    if(nthreads > 0)
      sem_post(&nthreadsactive);
    return NULL;
  }

  answer.i = request->i;
  answer.pid = pid;
  answer.tid = tid;
  answer.dur = request->dur;
  if(get_exec_time(start) + answer.dur * 0.0001 < (double) nsecs && opened) {
    if(nplaces > 0) {
      sem_wait(&nplacesactive);
      answer.pl = getFreePlace();
    } else {
      answer.pl = place_i;
      place_i++;
    }
  } else {
    answer.pl = -1;
  }

  sendAnswer(answer, private_fd);

  if(answer.pl != -1) {
    usleep(request->dur * 1000);
    register_message(&answer, "TIMUP");
    places[answer.pl] = 0;
    sem_post(&nplacesactive);
  }

  close(private_fd);

  if(nthreads > 0)
    sem_post(&nthreadsactive);

  return NULL;
}

int main(int argc, char *argv[]) {
  struct tms t;
  char *fifoname;
  int rcode;

  fifoname = (char *) malloc(MAX * sizeof(char));
  get_options_Q2(argc, argv, &nsecs, fifoname, &nplaces, &nthreads);

  create_public_FIFO(fifoname);
  open_public_FIFO(fifoname);

  start = times(&t); // Inicio da medicao do tempo
  place_i = 1;
  opened = 1;

  if(nthreads > 0)
    sem_init(&nthreadsactive, 0, nthreads);

  if(nplaces > 0) {
    places = (int *) calloc(nplaces, sizeof(int));
    sem_init(&nplacesactive, 0, nplaces);
  }

  do {
    pthread_t tid;
    message_t request;

    if(get_exec_time(start) > (double) nsecs) {
      opened = 0;
    }

    rcode = read(public_fd, &request, sizeof(request));
    if(rcode == -1) {
      printf("Error receiving Order!\n");
    } else if(rcode > 0) {
      if (nthreads > 0) {
        sem_wait(&nthreadsactive);
      }
      pthread_create(&tid, NULL, server, &request);
    }
  } while(rcode > 0);

  close(public_fd);
  unlink(fifoname);

  free(fifoname);
  free(places);

  return 0;
}

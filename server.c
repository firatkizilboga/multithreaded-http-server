#include "blg312e.h"
#include "request.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>

//
// server.c: A very, very simple web server
//
// To run:
//  server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// blg312e: Parse the new arguments too
int arg = 1;
void getargs(int *port, int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <port> <threads> <buffers>\n", argv[0]);
    exit(1);
  }
  *port = atoi(argv[arg]);
  arg += 1;
}

int *connfd_buff = NULL;
sem_t connfd_buff_top;

int num_threads = 0; // maybe a semaphore would be better
sem_t buff_mutex;
int connfd_buff_size = 0;

int atomic_push_to_buff(int connfd) {
  sem_wait(&buff_mutex);

  printf("atomic pop from buffer\n");
  int top;
  sem_getvalue(&connfd_buff_top, &top);

  if (top >= connfd_buff_size) {
    sem_post(&buff_mutex);
    return -1;
  }

  connfd_buff[top] = connfd;
  sem_post(&connfd_buff_top);
  sem_post(&buff_mutex);
  return 0;
}

int atomic_pop_from_buff() {
  sem_wait(&connfd_buff_top);
  sem_wait(&buff_mutex);
  printf("atomic pop from buffer\n");
  int top;
  sem_getvalue(&connfd_buff_top, &top);
  int val = connfd_buff[top];

  sem_post(&buff_mutex);
  return val;
}

sem_t thread_num_mutex;
pthread_t atomic_get_new_thread(){
    sem_wait(&thread_num_mutex);
    printf("get new thread\n");
    pthread_t tid;
    return tid;
}


int listenfd, connfd, port, clientlen;
struct sockaddr_in clientaddr;

void* handler(){
    printf("handler\n");
    while (1) {
        pthread_t tid = atomic_get_new_thread();
        int*fd = (int*) malloc(sizeof(int));
        *fd = atomic_pop_from_buff();
        if (pthread_create(&tid, NULL, &requestHandle, fd) == 0){
            pthread_detach(tid);
        }
    }
    return NULL;
}

void *listener(){
    printf("listener\n");
    listenfd = Open_listenfd(port);
    while (1) {
        clientlen = sizeof(clientaddr);
        int fd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *)&clientlen);
        atomic_push_to_buff(fd);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
  
  getargs(&port, argc, argv);
  getargs(&num_threads, argc, argv);
  getargs(&connfd_buff_size, argc, argv);
  printf("%d, %d, %d\n", port, num_threads, connfd_buff_size);
  sem_init(&buff_mutex, 0, 1);
  sem_init(&thread_num_mutex, 0, num_threads);
  sem_init(&connfd_buff_top, 0, 0);

  connfd_buff = (int *)malloc(sizeof(int) * connfd_buff_size);

  if (connfd_buff == NULL) {
    fprintf(stderr, "Error: malloc failed\n");
    exit(1);
  }

  pthread_t handler_tid;

  if (pthread_create(&handler_tid, NULL, &handler, NULL) == 0) pthread_detach(handler_tid);
  else exit(1);

 
  listener();
  return 1;
}



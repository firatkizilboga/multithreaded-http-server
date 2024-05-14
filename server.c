#include "blg312e.h"
#include "request.h"
#include <pthread.h>
#include <sys/semaphore.h>
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
void getargs(int *port, int argc, char *argv[])
{
    if (argc != 2) {
		fprintf(stderr, "Usage: %s <port>\n", argv[0]);
		exit(1);
    }
    *port = atoi(argv[1]);
}

int * connfd_buff = NULL;
int connfd_buff_top = 0;
int num_threads = 0; // maybe a semaphore would be better
sem_t buff_mutex;

int atomic_push_to_buff(int connfd){
    if (connfd_buff_top >= num_threads){
        return -1;
    }

    connfd_buff[connfd_buff_top] = connfd;
    connfd_buff_top++;
    return 0;
}

int atomic_pop_from_buff(){
    if (connfd_buff_top <= 0){
        return -1;
    }

    connfd_buff_top--;
    return connfd_buff[connfd_buff_top];
}

int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen;
    int connfd_buff_size = 0;
    struct sockaddr_in clientaddr;

    getargs(&port, argc, argv);
    getargs(&num_threads, argc, argv);
    getargs(&connfd_buff_size, argc, argv);
    sem(&buff_mutex, 0, 1);

    connfd_buff = (int *) malloc(sizeof(int) * connfd_buff_size);

    if (connfd_buff == NULL){
        fprintf(stderr, "Error: malloc failed\n");
        exit(1);
    }

    listenfd = Open_listenfd(port);
    while (1) {
		clientlen = sizeof(clientaddr);
        connfd_ptr = (int *) malloc(sizeof(int));
        if (connfd_ptr == NULL){
            continue;
        }

        pthread_t tid;
		*connfd_ptr = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
        
        if (pthread_create(&tid, NULL, &requestHandle, connfd_ptr) != 0  || *connfd_ptr < 0){
            free(connfd_ptr);
            continue;
        }

        pthread_detach(tid);
    }
    exit(1);
}


    


 

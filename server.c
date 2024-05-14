#include "blg312e.h"
#include "request.h"
#include <pthread.h>
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


int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen;
    struct sockaddr_in clientaddr;

    getargs(&port, argc, argv);

    // 
    // blg312e: Create some threads...
    //
    int * connfd_ptr;
    
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
}


    


 

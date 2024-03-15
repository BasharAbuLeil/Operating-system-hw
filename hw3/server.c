#include "segel.h"
#include "request.h"
#include "queue.h"
#include "stdbool.h"
#define FAILED -1
// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// HW3: Parse the new arguments too
void reqManging();
void freeResources(pthread_t * threads, thread_T * threadsAbout);

pthread_mutex_t lock;
pthread_cond_t pendingCond, noPlaceCond, fsh7daCond;
int pendingLength ,handlingLength;
pendingLength=0;
handlingLength=0;
Queue* pendingQue ,handlingQue ;
handlingQue=NULL;
pendingQue=NULL;

void getargs(int *port, int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
}






int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen;
    struct sockaddr_in clientaddr;
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&pendingCond,NULL);
    pthread_cond_init(&fsh7daCond,NULL);
    pthread_cond_init(&noPlaceCond,NULL);
    getargs(&port, argc, argv);
    handlingQue=buildQueue(atoi(argv[2]));
    pendingQue=buildQueue(atoi(argv[3]));
    char * schedMod=argv[4];
    int numberOfThreads=atoi(argv[2]);
    thread_T * threadsAbout=(thread_T*)malloc(numberOfThreads*sizeof(*threadsAbout));
    pthread_t* threads=(pthread_t*)malloc(numberOfThreads*sizeof(*threads));
    buildThreads(threads,atoi(argv[2]),threadsAbout);
    for (int i = 0; i < numberOfThreads; i++){
        pthread_create(&threads[i],NULL,reqManging,(void*)&threadsAbout[i]);
    }
    
    

    listenfd = Open_listenfd(port);
    if(listenfd==FAILED){
        freeResources(threads,threadsAbout);
        return 0;

    }
    while (1) {////////////////////////////////////////////////////////// fe mloahda hon
	    clientlen = sizeof(clientaddr);
	    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
       
        request * req=(request*)malloc(sizeof(*req));
        req->m_fd=connfd;
        // timezone struct is not detected.
        gettimeofday(&req->m_arrival,NULL);
        //adding request Section
        pthread_mutex_lock(&lock);
        if(strcmp(schedMod,"dt")==0){

        }
        if(strcmp(schedMod,"random") == 0){

        }
        if(strcmp(schedMod,"block")==0){
            while(handlingQue->m_length+pendingQue->m_length==atoi(argv[3]))
                pthread_cond_wait(&noPlaceCond,&lock);
        }
        if(strcmp(schedMod,"dh")==0){

        }
        if(strcmp(schedMod,"bf") == 0){

        }

    }
    freeResources(threads,threadsAbout);
    return 0;

}


    


 
void freeResources(pthread_t * threads, thread_T * threadsAbout){
    demoilshQueue(pendingQue);
    demoilshQueue(handlingQue);
    free(threadsAbout);
    free(threads);
    pthread_mutex_destroy(&lock);
}
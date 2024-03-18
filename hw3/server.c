#include "segel.h"
#include "request.h"
#include"queue.h"
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
void* reqManging(void* arguments);
void appendingReq(request * req,int queLength,char * schedMod);
void freeResources(pthread_t * threads, thread_T * threadsAbout);

// HW3: Parse the new arguments too
int pendingLength,handlingLength;
pthread_mutex_t lock;
pthread_cond_t pendingCond, noPlaceCond, fsh7daCond;
handlingLength=0;
Queue* pendingQue =NULL;
Queue* handlingQue=NULL;
pendingLength=0;
void getargs(int *port, int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
}

int cmp(const void* x, const void* y){
    if(*(int*)x > *(int*)y){
        return 1;
    }
    return -1;
}

int getHalf(int x){
    int a = ((x%2) == 0) ? 0 : 1;
    return x/2 +a;
}

void dropRandom(){
    /*int half = getHalf(pendingLength);
    int *toDrop =(int*)malloc(sizeof(int)*half);
    bool *used = (bool*)malloc(sizeof(bool)*(pendingLength));
    for(int i=0;i<pendingLength;++i){
        used[i]=false;
    }
    for (int i = 0; i < half; i++){
        toDrop[i] = rand() % pendingLength;
        while(used[toDrop[i]])
            toDrop[i] = rand() % pendingLength;
        used[toDrop[i]] = true;
    }
    free(used);
    qsort(toDrop,half,sizeof(int),cmp);//Note that using qsort in O(nlogn) in average does not affect the worst case.
    int waitingQueueIndex = 0;
    int toDropIndex = 0;
    request* request;
    //delete half of the workingQueue size
     QueueNode curr = pendingQue->m_first,*temp;
    while (toDropIndex < half){
        if(toDrop[toDropIndex] == waitingQueueIndex){
            temp = curr;
            curr= curr->m_next;
            request = deleteNode(pendingQue,temp);
            Close(request->m_fd);
            toDropIndex++;
            pendingLength--;
        }else{
            curr = curr->m_next;
        }
        waitingQueueIndex++;
    }//update workingQueue size
    free(toDrop);*/
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
        struct timezone t;
        gettimeofday(&req->m_arrival,&t);
        appendingReq(req,atoi(argv[3]),schedMod);
        //adding request Section
        /*int pendingQueSize=atoi(argv[3]);
        pthread_mutex_lock(&lock);

        if(strcmp(schedMod,"dt")==0){
            if(handlingQue->m_length+pendingQue->m_length==pendingQueSize){
                Close(req->m_fd);
            }
            else{
                enqueue(pendingQue,req);
                pthread_cond_signal(&pendingCond);
            }
            
        }
        if(strcmp(schedMod,"random") == 0){
            /////////////bouns
        }
        if(strcmp(schedMod,"block")==0){/////////////////////done
            while(handlingQue->m_length+pendingQue->m_length==pendingQueSize)
                pthread_cond_wait(&noPlaceCond,&lock);
            enqueue(pendingQue,req);
            pthread_cond_signal(&pendingCond);
        }
        if(strcmp(schedMod,"dh")==0){
            if(handlingQue->m_length+pendingQue->m_length==pendingQueSize){
                if(pendingQue->m_length != 0) {
                    request* tempReq = dequeue(pendingQue);
                    Close(tempReq->m_fd);
                    enqueue(pendingQue,req);
                }
                else{
                    Close(req->m_fd);
                }
            }
            else{
                enqueue(pendingQue,req);
                pthread_cond_signal(&pendingCond);
            }
            
        }
        if(strcmp(schedMod,"bf") == 0){
            if(handlingQue->m_length+pendingQue->m_length==pendingQueSize) {
                while(pendingQue->m_length + handlingQue->m_length != 0){
                    pthread_cond_wait(&fsh7daCond, &lock);
                }
                Close(req->m_fd);
            }
            else{
                enqueue(pendingQue,req);
                pthread_cond_signal(&pendingCond);
            }
        }
        pthread_mutex_unlock(&lock);
    */
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



void* reqManging(void* arguments){
    thread_T* thread = (thread_T*)arguments;
    request* req;
    while(1){
        pthread_mutex_lock(&lock);
        while(pendingLength == 0)  pthread_cond_wait(&pendingCond,&lock);
        req = dequeue(pendingQue);
        pendingLength--;
        //struct timezone t;
        gettimeofday(&req->m_dispatch,NULL);
        timersub(&req->m_dispatch,&req->m_arrival,&req->m_dispatch);
        req->m_thread = thread;
        //enqueue(pendingQue,req);
        enqueue(handlingQue,req);// error
        handlingLength++;
        pthread_mutex_unlock(&lock);
        //req->m_thread->m_count++;
        requestHandle(req);
        pthread_mutex_lock(&lock);
        //deleteNode(pendingQue,getNode(handlingQue,req));// error
        deleteNode(handlingQue,getNode(handlingQue,req));
        pthread_cond_signal(&noPlaceCond);        
        Close(req->m_fd);
        if(pendingLength + handlingLength== 0) pthread_cond_signal(&fsh7daCond);
        pthread_mutex_unlock(&lock);
    }
    return NULL;
    
}




void appendingReq(request * req,int queLength,char * schedMod){
    pthread_mutex_lock(&lock);
    bool ignore = false;

    if(strcmp(schedMod,"block") == 0){
        while(pendingLength+handlingLength == queLength) {
            pthread_cond_wait(&noPlaceCond, &lock);
        }
    }
    if(strcmp(schedMod,"dt") == 0){
        if(pendingLength+handlingLength == queLength) {
            ignore = true;
        }
    }
    if(strcmp(schedMod,"dh") == 0){
        if(pendingLength+handlingLength == queLength){
            if(pendingLength != 0) {
                request*  temp = dequeue(pendingQue);
                Close(temp->m_fd);
                enqueue(pendingQue,req);
                pthread_mutex_unlock(&lock);
                return;
            }else{
                ignore = true;
            }
        }
    }
    if(strcmp(schedMod,"bf") == 0){
        if(pendingLength+handlingLength == queLength) {
            ignore = true;
            while(pendingLength+handlingLength == queLength){
                pthread_cond_wait(&fsh7daCond, &lock);
            }

        }
        
    }
    /*if(strcmp(schedMod,"random") == 0) {
        if(pendingLength+handlingLength == queLength){
            if(pendingLength != 0){
                dropRandom();
            }else{
                ignore = true;
            }
        }
    }*/
    if(!ignore) {
        enqueue(pendingQue, req);
        pendingLength++;
        pthread_cond_signal(&pendingCond);
    }else{
        Close(req->m_fd);
    }
    pthread_mutex_unlock(&lock);
}
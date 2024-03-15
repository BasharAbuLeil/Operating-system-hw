#include "queue.h"




Queue * buildQueue(int maxCap){
    Queue* head= malloc(sizeof(*head));
    if(!head){
        return NULL;
    }
    head->m_first=NULL;
    head->m_length=0;
    head->m_maxCapacity=maxCap;
    head->m_tail=NULL;
    return head;
}

void enqueue(Queue * que, request* r1){
    if(!que)
        return;
    QueueNode temp=malloc(sizeof(*temp));
    if(!temp)
        return;
    temp->m_req=r1;
    if(que->m_length==0){
        temp->m_next=NULL;
        temp->m_previous=NULL;
        que->m_first=temp;
        que->m_tail=temp;

    }
    else{
        temp->m_previous=que->m_tail;
        temp->m_next=NULL;
        que->m_tail->m_next=temp;
        que->m_tail=temp;
    }
    que->m_length++;
}

request* dequeue(Queue* que){
    if(!que)
        return NULL;
    return deleteNode(que,que->m_first);
}

QueueNode getNode(Queue* que,request* r){
    if(!que)
        return NULL;
    QueueNode iterator=que->m_first;
    while(iterator){
        if(iterator->m_req->m_fd==r->m_fd){
            return iterator;
        }
        iterator=iterator->m_next;
    }
    return NULL;
}


void demoilshQueue(Queue* que){
    if(!que)
        return;
    QueueNode iterator=que->m_first;
    while(iterator){
        que->m_first=que->m_first->m_next;
        free(iterator->m_req);
        free(iterator);
        iterator=que->m_first;
    }
}


request* deleteNode(Queue* que,QueueNode node){
    if(!que||que->m_length==0||!node)
        return NULL;
    if(que->m_length==1){
        free(node);
        que->m_length--;
        que->m_first=que->m_tail=NULL;
    }
    if(que->m_first==node){
        que->m_first=que->m_first->m_next;
        que->m_first->m_previous=NULL;
        free(node);
        que->m_length--;
        return;
    }
    else if(que->m_tail==node){
        que->m_tail=que->m_tail->m_previous;
        que->m_tail->m_next=NULL;
        free(node);
        que->m_length--;
        return;
    }
    QueueNode nodePrev=node->m_previous;
    QueueNode nodeNext=node->m_next;
    nodePrev->m_next=nodeNext;
    nodeNext->m_previous=nodePrev;
    free(node);
    que->m_length--;
}







void buildThreads(pthread_t* threads,int size,thread_T* threadAbout){
    for(int i=0;i<size;++i){
        threadAbout[i].m_thread = &threads[i];
        threadAbout[i].m_dynamic=0;
        threadAbout[i].m_count=0;
        threadAbout[i].m_static=0;
        threadAbout[i].m_id=i;
    }
}
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
    if(!temp||!r1)
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
    request* temp= node->m_req;
    if(que->m_length==1){
        free(node);
        que->m_length--;
        que->m_first=que->m_tail=NULL;
        return temp;
    }
    if(que->m_first==node){
        que->m_first=que->m_first->m_next;
        que->m_first->m_previous=NULL;
        free(node);
        que->m_length--;
        return temp;
    }
    else if(que->m_tail==node){
        que->m_tail=que->m_tail->m_previous;
        que->m_tail->m_next=NULL;
        free(node);
        que->m_length--;
        return temp;
    }
    QueueNode nodePrev=node->m_previous;
    QueueNode nodeNext=node->m_next;
    nodePrev->m_next=nodeNext;
    nodeNext->m_previous=nodePrev;
    free(node);
    que->m_length--;
    return temp;
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
/*requestInfo errorValue(){
    requestInfo r;
    r.fd = -1;
    r.thread = NULL;
    return r;
}

Queue *initQueue() {
    Queue *newQueue = malloc(sizeof(Queue));
    if (newQueue == NULL) {
        return NULL;
    }
    newQueue->size = 0;
    newQueue->head = newQueue->last = NULL;
    return newQueue;
}

QueueStatus push(Queue* queue,T val){
    if(queue == NULL){
        return NULL_ARG;
    }
    Node* newNode = (Node*)malloc(sizeof(Node));
    
    if(newNode == NULL){
        return MALLOC_FAIL;
    }
    newNode->next=newNode->prev=NULL;
    if(queue->head ==NULL){
        queue->head =queue->last = newNode;

    }else{
        queue->last->next = newNode;
        newNode->prev = queue->last;
        queue->last = newNode;
    }
    newNode->next = NULL;
    newNode->data = val;
    queue->size++;
    return SUCCESS;
}


T pop(Queue* queue){
    return removeNode(queue,queue->head);
}

T removeNode(Queue* queue, Node* node){
    if(queue == NULL || queue->size ==0 || node == NULL){
        return errorValue();
    }
    T result = node->data;
    if(node->prev ==NULL){//first
        if(node->next ==NULL){//and last
            queue->head = queue->last = NULL;
        }else{
            node->next->prev = NULL;
            queue->head = node->next;
        }
    }else{//last
        if(node->next ==NULL){
            if(node->prev ==NULL){//and last
                queue->head = queue->last = NULL;
            }else{
                node->prev->next = NULL;
                queue->last = node->prev;
            }
        }else{
            node->next->prev =node->prev;
            node->prev->next = node->next;
        }
    }
    queue->size--;
    free(node);
    return result;
}


QueueStatus destroyQueue(Queue* queue){
    if(queue == NULL){
        return NULL_ARG;
    }
    Node* curr = queue->head;
    while(curr != NULL){
        Node* temp = curr->next;
        free(curr);
        curr = temp;
    }
    return SUCCESS;
}

Node* findNode(Queue* queue, T data){
    if(queue == NULL){
        return NULL;
    }
    Node* curr = queue->head;
    while(curr){
        if(curr->data.fd == data.fd){
            break;
        }
        curr=curr->next;
    }
    return curr;
}*/
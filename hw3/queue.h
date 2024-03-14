#ifndef QUEUE_H
#define QUEUE_H
#include "segel.h"

typedef struct thread_T
{
    int m_dynamic;
    pthread_t * m_thread;
    int m_static;
    int m_count;
    int m_id;
} thread_T;

typedef struct{
    int m_fd ;
    struct timeval m_arrival;
    struct timeval m_dispatch;
    thread_T * m_thread;
}request;

typedef struct queueNode{
    request* m_req;
    struct queueNode* m_next;
    struct queueNode* m_previous;

}*QueueNode;

typedef struct{
    QueueNode m_first;
    QueueNode m_tail;
    int m_length;
    int m_maxCapacity;
}Queue;

Queue * buildQueue(int maxCap);

void enqueue(Queue * que, request* r1);

request* dequeue(Queue* que);

void demoilshQueue(Queue* que);

QueueNode getNode(Queue* que,request* r);

request* deleteNode(Queue* que,QueueNode node);














#endif
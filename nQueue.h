//
// Created by Hadas on 6/8/2021.
//

#ifndef OS3NEW_NQUEUE_H
#define OS3NEW_NQUEUE_H
#include "segel.h"
struct QueueNode
{
    int val;
    struct timeval time_arrive;
    struct timeval disp;
    struct QueueNode *next;
    struct QueueNode *prev;
}QueueNode;

struct QueueNode *createQueueNode(int val);

typedef struct Queue
{
    struct QueueNode *head;
    struct QueueNode *tail;
    struct QueueNode *last_req_run_q;
    int max_queue_size;
    int queue_size;
    int wait_q_size;

}Queue;

Queue *createQueue(int q_size);
void enqueue(Queue* q, int val);
void dequeue(Queue *q, int val);
int DeleteOldWaitReq(Queue* q);
struct QueueNode* addFirstReqToRun(Queue* q);
int deleteWaitReqByNum(Queue* q,int num);
void DeleteQueue(Queue* q);
///change hadas
int IsWaitEmpty(Queue* q);
int SameQueueSizeCond(Queue* q);


#endif //OS3NEW_NQUEUE_H

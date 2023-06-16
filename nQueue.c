//
// Created by Hadas on 6/8/2021.
//
#include "nQueue.h"

struct QueueNode *createQueueNode(int val)
{
    struct QueueNode *_nNode = (struct QueueNode *)malloc(sizeof(struct QueueNode));
    _nNode->val = val;
    _nNode->next = NULL;
    _nNode->prev = NULL;
    return _nNode;
}

struct Queue *createQueue(int q_size)
{
    Queue *q = (Queue *)malloc(sizeof(Queue));

    q->head = createQueueNode(0);
    q->tail = createQueueNode(0);

    q->head->next = q->tail;
    q->tail->prev = q->head;
    q->last_req_run_q = q->head;

    q->max_queue_size=q_size;
    q->queue_size = 0;
    q->wait_q_size = 0;

    return q;
}
///hadas change
int IsWaitEmpty(Queue* q){
    if(q->wait_q_size==0){
        return 1;
    }
    return 0;
}
int SameQueueSizeCond(Queue* q){
    if(q->queue_size < q->max_queue_size){
        return 1;
    }
    return 0;
}
void dispTime( struct timeval *start, struct timeval *finish,struct timeval *ans)
{

    if (finish->tv_usec > 999999)
    {
        finish->tv_sec += finish->tv_usec / 1000000;
        finish->tv_usec %= 1000000;
    }

    if (start->tv_usec > 999999)
    {
        start->tv_sec += start->tv_usec / 1000000;
        start->tv_usec %= 1000000;
    }

    ans->tv_sec = finish->tv_sec - start->tv_sec;
    ans->tv_usec = finish->tv_usec - start->tv_usec;

    if (ans->tv_usec< 0)
    {
        ans->tv_usec += 1000000;
        ans->tv_sec--;
    }
}

void enqueue(Queue* q, int val)
{
    struct QueueNode *node_to_add;
    struct QueueNode *temp;

    node_to_add = createQueueNode(val);
    temp = q->tail->prev;

    q->tail->prev = node_to_add;
    node_to_add->next = q->tail;
    node_to_add->prev = temp;
    temp->next = node_to_add;

    gettimeofday(&(node_to_add->time_arrive),NULL);
    q->queue_size++;
    q->wait_q_size++;
}

void dequeue(Queue *q, int val)
{
    struct QueueNode *temp = q->head->next;


    temp->next->prev=temp->prev;
    temp->prev->next=temp->next;

    if(q->last_req_run_q==temp){
        q->last_req_run_q = temp->prev;
    }

    if(q->head->next==q->tail){
        q->last_req_run_q=q->head;
    }

    q->queue_size--;
    free (temp);
    return;
}
int DeleteOldWaitReq(Queue* q){

    struct QueueNode *old_wait_req;
    q->queue_size--;
    q->wait_q_size--;
//switch old and last run
    old_wait_req = q->last_req_run_q->next;
    q->last_req_run_q->next = old_wait_req->next;

    old_wait_req->next->prev = q->last_req_run_q;

    if(q->head->next==q->tail){
        q->last_req_run_q=q->head;
    }

    int old_req_val=old_wait_req->val;
    free (old_wait_req);
    return old_req_val;
}
struct QueueNode* addFirstReqToRun(Queue* q){
    struct QueueNode* firstWaiting=q->last_req_run_q->next;


    struct timeval time_now;
    gettimeofday(&time_now,NULL);

    q->wait_q_size --;
    q->last_req_run_q = q->last_req_run_q->next;
    dispTime(&(firstWaiting->time_arrive),&time_now,&(firstWaiting->disp));

    return firstWaiting;
}


int deleteWaitReqByNum(Queue* q,int num){
    struct QueueNode *toRemove = q->last_req_run_q->next;
    for (int i = 0; i < num; i++)
    {
        toRemove=toRemove->next;
    }
    //do we have here one more loop that we dont need?
    q->queue_size--;
    q->wait_q_size--;

    toRemove->prev->next=toRemove->next;
    toRemove->next->prev=toRemove->prev;


    int retdata=toRemove->val;
    free (toRemove);
    return retdata;
}
void DeleteQueue(Queue* q){
    struct QueueNode *toRemove=q->head;
    if(!toRemove){
        return;
    }
    while(toRemove){
        struct QueueNode *next_remove = toRemove;
        toRemove = next_remove->next;
        free(next_remove);
    }
    free(q);

}
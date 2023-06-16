
#include "segel.h"
#include "request.h"
#include "Thread.h"
#include "nQueue.h"

// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//
#define MAXSCHHEDALGLEN 7

pthread_mutex_t lock;
pthread_cond_t isEmpty;
pthread_cond_t isFull;
struct Queue *webReqQueue;
// HW3: Parse the new arguments too
void getargs(int *port, int *threads, int *queue_size, char *schedalg, int argc, char *argv[])
{
    if (argc < 5)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    *port = atoi(argv[1]);
    *threads = atoi(argv[2]);
    *queue_size = atoi(argv[3]);
    strcpy(schedalg, argv[4]);
}
void *threadRoutine(void *thread)
{
    Thread *_nthread = (Thread *)thread;
    while (1)
    {

        pthread_mutex_lock(&lock);
        ///change hadas
        while (IsWaitEmpty(webReqQueue))
        {
            pthread_cond_wait(&isEmpty, &lock);
        }

        struct QueueNode *first_wait_q = addFirstReqToRun(webReqQueue);
        struct timeval time_arrive = first_wait_q->time_arrive;
        struct timeval disp = first_wait_q->disp;
        int fd = first_wait_q->val;


        pthread_mutex_unlock(&lock);
        requestHandle(fd,_nthread, time_arrive, disp);
        Close(fd);
        //critic code
        pthread_mutex_lock(&lock);
        dequeue(webReqQueue, fd);
        pthread_cond_signal(&isFull);
        pthread_mutex_unlock(&lock);
     //end critic code
    }
}

void Initialize(int queue_size,int threads){

    webReqQueue = createQueue(queue_size);

    pthread_t *thread_pool = (pthread_t *)malloc(sizeof(pthread_t) * threads);
    memset(thread_pool, 0, threads * sizeof(thread_pool[0]));

    for (int i = 0; i < threads; i++)
    {
        Thread * thread = threadCreate(i);
        if (pthread_create(&(thread_pool[i]), NULL, &threadRoutine, (void*)thread) != 0)
        {
            fprintf(stderr, "pthread_create failed\n");
            exit(1);
        }
    }
}
void AddToQ(int connfd){
    enqueue(webReqQueue, connfd);
    pthread_cond_signal(&isEmpty);
    pthread_mutex_unlock(&lock);
}

int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen,threads,queue_size;
    struct sockaddr_in clientaddr;
    char* schedalg = (char*)malloc(MAXSCHHEDALGLEN);

    getargs(&port,&threads,&queue_size,schedalg, argc, argv);
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&isEmpty, NULL);
    pthread_cond_init(&isFull, NULL);
    Initialize(queue_size,threads);
    listenfd = Open_listenfd(port);


    while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
    pthread_mutex_lock(&lock);
    ///hadas change
    if (SameQueueSizeCond(webReqQueue)){
        AddToQ(connfd);
    }
    else{
        if(!strcmp(schedalg,"block")){
            ///change hadas
            while(!SameQueueSizeCond(webReqQueue)){
                pthread_cond_wait(&isFull, &lock);
            }
            AddToQ(connfd);
        }else if(!strcmp(schedalg,"dt")){
            ///change hadas
            if(!SameQueueSizeCond(webReqQueue)){
                Close(connfd);
                pthread_mutex_unlock(&lock);
                continue;
            }
            //AddToQ(connfd);//shouldnot get here
        }else if(!strcmp(schedalg, "dh")){
            if(!SameQueueSizeCond(webReqQueue)){

                ///change hadas
                if(IsWaitEmpty(webReqQueue)){
                    Close(connfd);
                    pthread_mutex_unlock(&lock);
                    continue;
                }
                Close(DeleteOldWaitReq(webReqQueue));
                AddToQ(connfd);
            }
        } else //random
        {
            if (!SameQueueSizeCond(webReqQueue)) {
                ///change hadas
                if (IsWaitEmpty(webReqQueue)) {
                    Close(connfd);
                    pthread_mutex_unlock(&lock);
                    continue;
                }

                double quarter_wait_q = (((double) webReqQueue->wait_q_size) / 4);
                double num_to_delete = ceil((quarter_wait_q));
                for (int i = 0; i < num_to_delete; i++) {
                    //rand between the queue size
                    int num = rand() % webReqQueue->wait_q_size;

                    int connfd_to_close = deleteWaitReqByNum(webReqQueue, num);
                    Close(connfd_to_close);
                }
                AddToQ(connfd);
            }
        }
    }

	// 
	// HW3: In general, don't handle the request in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work. 
	//

    }
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&isEmpty);
    pthread_cond_destroy(&isFull);
    DeleteQueue(webReqQueue);

}


    


 

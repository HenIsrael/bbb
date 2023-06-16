//
// Created by Hadas on 6/8/2021.
//

#ifndef OS3NEW_THREAD_H
#define OS3NEW_THREAD_H
#include "segel.h"

typedef struct thread
{
    int thread_id;
    int thread_count;
    int thread_static;
    int thread_dynamic;

}Thread;

Thread* threadCreate(int thread_id);
#endif //OS3NEW_THREAD_H

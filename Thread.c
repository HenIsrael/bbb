//
// Created by Hadas on 6/8/2021.
//

#include "Thread.h"
Thread* threadCreate(int thread_id){
    Thread *_nThread = (Thread*)malloc(sizeof(Thread));
    if(!_nThread){
        return NULL;
    }
    _nThread->thread_id = thread_id;
    _nThread->thread_count=_nThread->thread_dynamic=_nThread->thread_static=0;

    return _nThread;

}
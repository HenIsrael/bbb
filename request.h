#ifndef __REQUEST_H__
#include "Thread.h"

void requestHandle(int fd,Thread * threads,struct timeval time_arrive,struct timeval disp);

#endif

#ifndef _EPOLLCALLBACK_H
#define _EPOLLCALLBACK_H

#pragma once
#include "Epoll.h"

using namespace std;

class EpollCallback {
public:
	EpollCallback(){}
	virtual ~EpollCallback(){}
	virtual void doEvent(struct epoll_event*){}
};


#endif // _EPOLLCALLBACK_H

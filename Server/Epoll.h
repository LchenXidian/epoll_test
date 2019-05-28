#ifndef _EPOLL_H
#define _EPOLL_H

#pragma once

#include <string>
#include <iostream>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdarg.h>
#include "EpollCallback.h"

using namespace std;

class EpollCallback;

class Epoll{
public:
    Epoll(int x = -1) : epollfd(x){}
    ~Epoll(){}

    void initEpoll(int maxEvents = 65535);
    int initServer(EpollCallback* cbk, std::string hostName, int port, int socketType = SOCK_STREAM);
    int connectServer(EpollCallback* cbk, std::string hostName, int port, int socketType = SOCK_STREAM);
    int socketBind(std::string hostName, int port, int socketType = SOCK_STREAM);
    std::string getIpByHost(std::string hostName, int port);
    bool add_Event(int fd, int state, EpollCallback* cbk);
    bool remove_Event(int fd, int state);
    bool modify_Event(int fd, int state, EpollCallback* cbk);
    void handleAccpet(int fd, int listenfd);
    void setNonBlocking(int fd);  //ÉèÎª·Ç×èÈû
    void doEpoll(int maxEvents, int timeout = -1);

private:
    int epollfd;
};



#endif // _EPOLL_H

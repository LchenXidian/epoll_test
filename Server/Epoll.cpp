#pragma once
#include "Epoll.h"

void Epoll::initEpoll(int maxEvents) {
    epollfd = epoll_create(maxEvents);  //若不设的话有默认值65535
	if (epollfd < 0) {
		perror("epoll create error:");
		exit(-1);
	}
}

int Epoll::initServer(EpollCallback* call, std::string hostName, int port, int socketType) {
	int listen_sock = socketBind(hostName, port, socketType);
	setNonBlocking(listen_sock);
	if (!add_Event(listen_sock, EPOLLIN , call)) {
        perror("addEvent fail error:");
		exit(-1);
	}
	return listen_sock;
}

int Epoll::connectServer(EpollCallback* call, std::string hostName, int port, int socketType) {
	int sockfd = socket(AF_INET, socketType, 0);
	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	std::string ip = getIpByHost(hostName, port);
	inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr);
	servaddr.sin_port = htons(port);
	int rt = connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr));
	if (rt < 0) {
		perror("connect error:");
		exit(-1);
	}
	string sendbuf;

	add_Event(sockfd, EPOLLIN, call);
	return sockfd;
}

int Epoll::socketBind(std::string hostName, int port, int socketType) {
	int listenfd;
	struct sockaddr_in servaddr;
	listenfd = socket(AF_INET, socketType, 0);
	if (listenfd < -1) {
        perror("socket error:");
		exit(-1);
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;  //IPv4
	std::string ip = getIpByHost(hostName, port);
	inet_pton(AF_INET, ip.c_str(), &servaddr.sin_addr);
	//servaddr.sin_addr.s_addr = inet_addr(hostName.c_str());
	servaddr.sin_port = htons(port);  //返回网络字节序的值
	if (bind(listenfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) == -1) {
        perror("bind error:");
		exit(-1);
	}
	//cout << servaddr.sin_addr.s_addr << " " << servaddr.sin_port << " " << port << endl;
	listen(listenfd, 1024);
	return listenfd;
}

void Epoll::setNonBlocking(int fd) {
	int flags, s;
	flags = fcntl(fd, F_GETFL);
	if (flags < 0) {
        perror("fcntl F_GETFL error:");
		exit(-1);
	}

	flags |= O_NONBLOCK;
	s = fcntl(fd, F_SETFL, flags);  //设置文件打开方式为flag指定方式
	if (s < 0) {
        perror("fcntl F_SETFL error:");
		exit(-1);
	}
}

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*) sa)->sin_addr);
	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}
std::string Epoll::getIpByHost(std::string hostName, int port) {
	struct addrinfo hints;
	struct addrinfo *result;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
    string p = to_string(port);
	int rt = getaddrinfo(hostName.c_str(), p.c_str(), &hints, &result);
	if (rt != 0) {
		perror("getaddrinfo error 1:");
        exit(-1);
	}
	if (result == NULL) {
        perror("getaddrinfo error 2:");
        exit(-1);
	}

	char ip[INET6_ADDRSTRLEN];
	inet_ntop(result->ai_family, get_in_addr(result->ai_addr), ip, sizeof(ip));
	freeaddrinfo(result);

	return ip;
}

void Epoll::doEpoll(int maxEvents, int timeout){
    struct epoll_event* events = (epoll_event*)calloc(maxEvents, sizeof(struct epoll_event));
    cout << "Waiting..." << endl;
	while (true) {
		int nfds = epoll_wait(epollfd, events, maxEvents, timeout);
		if (nfds < 0) {
            perror("epoll_wait error:");
            free(events);
            exit(-1);
		}
		for (int i = 0; i < nfds; ++i) {
			if (events[i].data.ptr) {
				EpollCallback* context = (EpollCallback*) events[i].data.ptr;
				context->doEvent(&events[i]);
			}
		}
	}
	free(events);
}

bool Epoll::add_Event(int fd, int state, EpollCallback* call) {
	struct epoll_event event;
	event.events = state;
	event.data.ptr = call;
	int rt = epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
	return rt == 0;
}

bool Epoll::remove_Event(int fd, int state) {
	struct epoll_event ev;
	ev.events = state;
	int rt = epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
	return rt == 0;
}

bool Epoll::modify_Event(int fd, int state, EpollCallback* call) {
	struct epoll_event event;
	event.events = state;
	event.data.ptr = call;
	int rt = epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
	return rt == 0;
}




#include "EpollCallback.h"

vector<EpollCallback*> clients;
char buff[1024];



void ClientCallback::doEvent(struct epoll_event* event) {  //文件描述符可读
    if (event->events & EPOLLIN) {
        int n = read(clientfd, buff, 1024);
        if (n <= 0) {
            cout << "close " << clientfd << endl;
            close(clientfd);
            epoll.removeEvent(clientfd, EPOLLIN); // | EPOLLLT
            ClientCallback* cltCallback = (ClientCallback*)event->data.ptr;
            event->data.ptr=NULL;
            delete cltCallback;
            return;
        }
        buff[n] = '\0';
        cout << clientfd << "read " << buff << endl;
        write(clientfd, buff, n);
    }
}

void ListenCallback::setListenfd(int listenfd) {
    this->listenfd = listenfd;
}
void ListenCallback::doEvent(struct epoll_event* event) {  //新的文件描述符
    struct sockaddr_in clientaddr;
    socklen_t clilen = 1;
    int clientfd = accept(listenfd, (sockaddr *) &clientaddr, &clilen);
    if (clientfd < 0) {
        perror("accept");
        std::cout << clientfd << std::endl;
        return;
    }
    char *str = inet_ntoa(clientaddr.sin_addr);
    std::cout << "accapt a connection from: " << str << " port: " << clientaddr.sin_port << std::endl;
    ClientCallback* cltCallback = new ClientCallback(epoll, clientfd, str);
    clients.push_back(cltCallback);
    epoll.addEvent(clientfd, EPOLLIN, cltCallback); // | EPOLLET
}

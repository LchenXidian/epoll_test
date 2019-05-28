#include "Epoll.h"
#include "EpollCallback.h"
#include <iostream>
#include <vector>
using namespace std;

vector<EpollCallback*> clients;
char buff[512];
class ClientCallback: public EpollCallback {
public:
	ClientCallback(Epoll &epoll, int clientfd, std::string clientIp):
                   epoll(epoll), clientfd(clientfd), clientIp(clientIp) {
	}

	virtual void doEvent(struct epoll_event* event) {  //文件描述符可读
		if (event->events & EPOLLIN) {
			int n = read(clientfd, buff, 1024);
			if (n <= 0) {
				cout << "close " << clientfd << endl;
				close(clientfd);
				epoll.remove_Event(clientfd, EPOLLIN);
				ClientCallback* cltCallback = (ClientCallback*)event->data.ptr;
				event->data.ptr=NULL;
				delete cltCallback;
				return;
			}
			buff[n] = '\0';
			cout << "read " << buff << endl;
			write(clientfd, buff, n);
		}
	}
private:
	Epoll &epoll;
	int clientfd;
	std::string clientIp;
};

class ListenCallback: public EpollCallback {
public:
	ListenCallback(Epoll &epoll):epoll(epoll), listenfd(-1){}
	void setListenfd(int listenfd) {
		this->listenfd = listenfd;
	}
	virtual void doEvent(struct epoll_event* event) {  //新的文件描述符
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
		epoll.add_Event(clientfd, EPOLLIN, cltCallback);
	}
private:
	Epoll &epoll;
	int listenfd;
};

int main(){
    Epoll epoll;
    epoll.initEpoll(2048);
    ListenCallback* listenCallback = new ListenCallback(epoll);
    int fd = epoll.initServer(listenCallback, "0.0.0.0", 5555);
    listenCallback->setListenfd(fd);
    epoll.doEpoll(2048);
    close(fd);
    delete listenCallback;

    return 0;
}

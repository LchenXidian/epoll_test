#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <cstdlib>
#include <vector>
#include <cstdio>
#include <mutex>
using namespace std;

#define SERV_PORT	5555
#define MAXLINE		512

void cli(int i){
   	//this_thread::sleep_for(chrono::seconds(1));
   	int sockfd, ret;
   	struct sockaddr_in seraddr;
    	struct sockaddr_in cliaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0); //AF_INET是IPv4协议. 字节流套接字

	if(sockfd == -1){
		perror("socket error");
		exit(-1);
	}
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(SERV_PORT);
	seraddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	//seraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//cout << seraddr.sin_addr.s_addr << " " << seraddr.sin_port << " " << SERV_PORT << endl;
	ret = connect(sockfd, (const struct sockaddr*)&seraddr, sizeof(seraddr));
	if(ret < 0){
		perror("connect error");
		return;
	}
	string sendbuf;
	sendbuf = to_string(i);

	size_t w = write(sockfd, sendbuf.c_str(), sendbuf.size());
	if(w == -1){
	    perror("socket error");
	    exit(-1);
	}
	//cout << "write:" << i << " " << w << endl;

	char line[MAXLINE];
	int t = read(sockfd, line, MAXLINE);
	line[t] = '\0';
}


int main(){
    vector<thread> client;
    for(int i = 0; i < 100; ++i){
        client.push_back(thread(cli, i));
    }
    for(auto& p:client)
        p.join();
    return 0;
}


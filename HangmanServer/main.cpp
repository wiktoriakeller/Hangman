#include "ServerHandler.h"
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <error.h>
#include <thread>
#include <string>
#include <sys/epoll.h>
#include <signal.h>
#include <map>
#include <vector>
#include "Game.h"

void ctrl_c(int);

int main() {
	ServerHandler* serverHandler = new ServerHandler();

	int epollFd = epoll_create1(0);
	signal(SIGINT, ctrl_c);

	epoll_event epollEvent{ EPOLLIN, {.ptr = serverHandler} };
	epoll_ctl(epollFd, EPOLL_CTL_ADD, Game::Instance().GetServerSocket(), &epollEvent);
	Game::Instance().SetEpollFd(epollFd);

	while (true) {
		if (-1 == epoll_wait(epollFd, &epollEvent, 1, -1)) {
			error(1, errno, "Epoll failed");
			ctrl_c(SIGINT);
		}

		((Handler*)epollEvent.data.ptr)->Handle(epollEvent.events);
	}
}

void ctrl_c(int) {
	close(Game::Instance().GetServerSocket());
	printf("Closing server\n");
	exit(0);
}

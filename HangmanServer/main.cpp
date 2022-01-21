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
#include <memory>
#include "Game.h"

void ctrl_c(int);

int main() {
	signal(SIGINT, ctrl_c);
	int epollFd = epoll_create1(0);
	std::shared_ptr<Handler> handler = std::make_shared<ServerHandler>(epollFd);
	std::shared_ptr<ServerHandler> serverHandler = std::dynamic_pointer_cast<ServerHandler>(handler);

	epoll_event epollEvent{ EPOLLIN, {.ptr = static_cast<void*>(handler.get()) }};
	epoll_ctl(epollFd, EPOLL_CTL_ADD, serverHandler->GetServerSocket(), &epollEvent);
	Game::Instance().SetServer(handler);
	Game::Instance().LoadWords();

	while (true) {
		if (-1 == epoll_wait(epollFd, &epollEvent, 1, -1)) {
			error(1, errno, "Epoll failed");
			printf("Epoll failed");
			break;
		}

		if (epollEvent.data.ptr != nullptr) {
			HandleResult result = (static_cast<Handler*>(epollEvent.data.ptr))->Handle(epollEvent.events);
			
			if (result == HandleResult::DeleteServer) {
				break;
			}
		}
	}

	Game::Instance().EndGame();
}

void ctrl_c(int) {
	printf("Closing server\n");
	Game::Instance().EndGame();
	exit(0);
}

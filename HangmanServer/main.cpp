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
			error(0, errno, "Epoll wait failed");
			break;
		}

		if (epollEvent.data.ptr != nullptr) {
			auto result = (static_cast<Handler*>(epollEvent.data.ptr))->Handle(epollEvent.events);
			
			if (std::get<0>(result) == HandleResult::DeletePlayer) {
				int id = std::get<1>(result);
				int roomId = std::get<2>(result);
				std::string name = std::get<3>(result);

				Game::Instance().DeletePlayer(id);
				Game::Instance().DeletePlayerFromRoom(roomId, name);
				printf("Player closed\n");

				std::shared_ptr<Room> room = Game::Instance().GetRoom(roomId);
				if (room != nullptr) {
					if (room->GetNumberOfPlayers() == 0)
						room->Close();
					Game::Instance().DeleteRoom(roomId);
				}
			}
			else if (std::get<0>(result) == HandleResult::DeleteRoom) {
				int roomId = std::get<1>(result);
				Game::Instance().DeleteRoom(roomId);
			}
			else if (std::get<0>(result) == HandleResult::DeleteServer) {
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

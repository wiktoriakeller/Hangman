#include "Game.h"

int Game::GetFreeRoomId() {
	for (int i = LOWER_BOUND; i <= UPPER_BOUND; i++) {
		if (!(roomMap.find(i) == roomMap.end())) {
			return i;
		}
	}
}

void Game::AddRoom(int id) {
	roomMap[id] = std::make_shared<Room>(id);
}

int Game::GetEpollFd() {
	return _epollFd;
}

int Game::GetServerSocket() {
	return _serverSocket;
}

void Game::SetEpollFd(int fd) {
	_epollFd = fd;
}

void Game::SetServerSocket(int fd) {
	_serverSocket = fd;
}

std::shared_ptr<Room> Game::GetRoom(int id) {
	if (roomMap.find(id) == roomMap.end())
		return roomMap[id];

	return nullptr;
}

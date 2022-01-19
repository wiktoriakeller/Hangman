#pragma once

#include <map>
#include "Room.h"

class Game {
public:
	static Game& Instance() {
		static Game instance;
		return instance;
	}

	int GetFreeRoomId();
	void AddRoom(int id);
	int GetEpollFd();
	int GetServerSocket();
	void SetEpollFd(int fd);
	void SetServerSocket(int fd);
	std::shared_ptr<Room> GetRoom(int id);

private:
	const int LOWER_BOUND = 0;
	const int UPPER_BOUND = 5000;
	int _epollFd;
	int _serverSocket;

	std::map<int, std::shared_ptr<Room>> roomMap;

	Game() {};
	Game(const Game&) = delete;
	Game(Game&&) = delete;
	void operator=(const Game&) = delete;
	void operator=(const Game&&) = delete;
};

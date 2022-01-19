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
	int GetFreePlayerId();
	void AddRoom(int id);
	std::shared_ptr<Room> GetRoom(int id);
	void AddPlayer(std::shared_ptr<Player> newPlayer, int id);
	std::shared_ptr<Player> GetPlayer(int id);
	void DeleteRoom(int id);
	void DeletePlayer(int id);
	void DeleteAllPlayers();
	void SetServer(std::shared_ptr<Handler> server);
	void EndGame();

private:
	const int LOWER_BOUND_ROOM = 0;
	const int UPPER_BOUND_ROOM = 5000;
	const int LOWER_BOUND_PLAYER = 0;
	const int UPPER_BOUND_PLAYER = 20000;

	std::shared_ptr<Handler> _server;
	std::map<int, std::shared_ptr<Room>> _roomsMap;
	std::map<int, std::shared_ptr<Player>> _playersMap;

	Game() {};
	Game(const Game&) = delete;
	Game(Game&&) = delete;
	void operator=(const Game&) = delete;
	void operator=(const Game&&) = delete;
};

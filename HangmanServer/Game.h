#pragma once
#include "Room.h"
#include <map>
#include <vector>
#include <limits>

class Game {
public:
	static Game& Instance() {
		static Game instance;
		return instance;
	}

	void LoadWords();
	std::string GetRandomWord();
	int GetFreeRoomId();
	int GetFreePlayerId();
	void AddRoom(int id, int epollFd);
	std::shared_ptr<Room> GetRoom(int id);
	void AddPlayer(std::shared_ptr<Player> newPlayer, int id);
	std::shared_ptr<Player> GetPlayer(int id);
	void DeleteRoom(int id);
	void DeletePlayer(int id);
	void DeletePlayerFromRoom(int roomId, std::string name);
	void DeleteAll();
	void SetServer(std::shared_ptr<Handler> server);
	void EndGame();
	bool DoesRoomExist(int id);
	void DeleteRoomAfterGame(int roomId);

private:
	const int LOWER_BOUND_ROOM = 0;
	const int UPPER_BOUND_ROOM = std::numeric_limits<int>::max();
	const int LOWER_BOUND_PLAYER = 0;
	const int UPPER_BOUND_PLAYER = std::numeric_limits<int>::max();;

	std::shared_ptr<Handler> _server;
	std::map<int, std::shared_ptr<Room>> _roomsMap;
	std::map<int, std::shared_ptr<Player>> _playersMap;
	std::vector<std::string> words;

	Game() {};
	Game(const Game&) = delete;
	Game(Game&&) = delete;
	void operator=(const Game&) = delete;
	void operator=(const Game&&) = delete;
};

#pragma once

#include "Player.h"
#include<memory>
#include<vector>

class Room {
public:
	Room(int id);
	int GetRoomId();
	void AddPlayer(std::shared_ptr<Player> player);
	std::shared_ptr<Player> GetPlayer(std::string name);
	void DeletePlayer(std::string name);
	void DeleteAllPlayersInRoom();

private:
	int _roomId;
	std::vector<std::shared_ptr<Player>> playersInRoom;
};

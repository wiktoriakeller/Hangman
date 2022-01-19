#pragma once

#include "Player.h"
#include<memory>
#include<vector>

class Room {
public:
	Room(int id);
	int GetRoomId();
	void AddPlayer(std::shared_ptr<Player> player);
private:
	int _roomId;
	std::vector<std::shared_ptr<Player>> players;
};
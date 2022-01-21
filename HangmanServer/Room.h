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
	void SendToAllBut(std::string message, std::string name);
	bool IsNameUnique(std::string name);
	bool IsRoomFull();
	std::string GetAllPlayerNamesBut(const std::string& name);

private:
	int _roomId;
	const int ROOM_MAX_SIZE = 6;
	std::vector<std::shared_ptr<Player>> _playersInRoom;
};

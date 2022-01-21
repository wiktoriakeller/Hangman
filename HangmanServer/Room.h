#pragma once

#include "Player.h"
#include<memory>
#include<map>

class Room {
public:
	Room(int id);
	int GetRoomId();
	void AddPlayer(std::shared_ptr<Player> player, std::string name);
	std::shared_ptr<Player> GetPlayer(const std::string& name);
	void DeletePlayer(std::string name);
	void DeleteAllPlayersInRoom();
	void SendToAllBut(std::string message, const std::string& name);
	bool IsNameUnique(const std::string& name);
	bool IsRoomFull();
	std::string GetSecretWord();
	std::string GetAllPlayerNamesBut(const std::string& name);

private:
	int _roomId;
	std::string _secretWord;
	const int ROOM_MAX_SIZE = 6;
	std::map<std::string, std::shared_ptr<Player>> _playersInRoom;
};

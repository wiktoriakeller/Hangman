#include "Room.h"

Room::Room(int id) {
	_roomId = id;
}

int Room::GetRoomId() {
	return _roomId;
}

void Room::AddPlayer(std::shared_ptr<Player> player) {
	_playersInRoom.emplace_back(player);
}

std::shared_ptr<Player> Room::GetPlayer(std::string name){
	for (int i = 0; i < _playersInRoom.size(); i++) {
		if (_playersInRoom[i]->GetName() == name)
			return _playersInRoom[i];
	}

	return nullptr;
}

void Room::DeletePlayer(std::string name) {
	int toDelte = -1;

	for (int i = 0; i < _playersInRoom.size(); i++) {
		if (_playersInRoom[i]->GetName() == name) {
			toDelte = i;
			break;
		}
	}

	if (toDelte != -1)
		_playersInRoom.erase(_playersInRoom.begin() + toDelte);
}

void Room::DeleteAllPlayersInRoom() {
	for (int i = _playersInRoom.size() - 1; i >= 0; i++) {
		_playersInRoom.pop_back();
	}
}

void Room::SendToAllBut(std::string message, std::string name) {
	for (int i = 0; i < _playersInRoom.size(); i++) {
		if (_playersInRoom[i]->GetName() != name)
			_playersInRoom[i]->PrepereToSend(message);
	}
}

bool Room::IsNameUnique(std::string name) {
	for (size_t i = 0; i < _playersInRoom.size(); i++) {
		if (_playersInRoom[i]->GetName() == name)
			return false;
	}

	return true;
}

bool Room::IsRoomFull() {
	if (_playersInRoom.size() == ROOM_MAX_SIZE)
		return true;
	return false;
}

std::string Room::GetAllPlayerNamesBut(const std::string& name) {
	std::string all = "";

	for (int i = 0; i < _playersInRoom.size(); i++) {
		if (_playersInRoom[i]->GetName() != name) {
			all += _playersInRoom[i]->GetName();
			all += " ";
		}
	}

	return all;
}

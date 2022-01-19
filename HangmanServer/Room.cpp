#include "Room.h"

Room::Room(int id) {
	_roomId = id;
}

int Room::GetRoomId() {
	return _roomId;
}

void Room::AddPlayer(std::shared_ptr<Player> player) {
	playersInRoom.emplace_back(player);
}

std::shared_ptr<Player> Room::GetPlayer(std::string name){
	for (int i = 0; i < playersInRoom.size(); i++) {
		if (playersInRoom[i]->GetName() == name)
			return playersInRoom[i];
	}

	return nullptr;
}

void Room::DeletePlayer(std::string name) {
	int toDelte = -1;

	for (int i = 0; i < playersInRoom.size(); i++) {
		if (playersInRoom[i]->GetName() == name) {
			toDelte = i;
			break;
		}
	}

	if (toDelte != -1)
		playersInRoom.erase(playersInRoom.begin() + toDelte);
}

void Room::DeleteAllPlayersInRoom() {
	for (int i = playersInRoom.size() - 1; i >= 0; i++) {
		playersInRoom.pop_back();
	}
}

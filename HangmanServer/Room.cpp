#include "Room.h"

Room::Room(int id) {
	_roomId = id;
}

int Room::GetRoomId() {
	return _roomId;
}

void Room::AddPlayer(std::shared_ptr<Player> player) {
	players.emplace_back(std::move(player));
}

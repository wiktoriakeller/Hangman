#include "Game.h"
#include <fstream>
#include <random>

void Game::LoadWords() {
	std::fstream wordsFile;
	wordsFile.open("Files//words.txt", std::ios::in);
	std::string word;

	if (wordsFile.is_open()) {
		while (wordsFile) {
			std::getline(wordsFile, word);
			if (word != "")
				words.emplace_back(word);
		}
	}
}

std::string Game::GetRandomWord() {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0, words.size());
	return words[dist(rng)];
}

int Game::GetFreeRoomId() {
	for (int i = LOWER_BOUND_ROOM; i <= UPPER_BOUND_ROOM; i++) {
		if (_roomsMap.find(i) == _roomsMap.end()) {
			return i;
		}
	}

	return -1;
}

int Game::GetFreePlayerId() {
	for (int i = LOWER_BOUND_PLAYER; i <= UPPER_BOUND_PLAYER; i++) {
		if (_playersMap.find(i) == _playersMap.end()) {
			return i;
		}
	}

	return -1;
}

void Game::AddRoom(int id) {
	_roomsMap[id] = std::make_shared<Room>(id);
}

std::shared_ptr<Room> Game::GetRoom(int id) {
	if (_roomsMap.find(id) != _roomsMap.end()) {
		return _roomsMap[id];
	}

	return nullptr;
}

void Game::AddPlayer(std::shared_ptr<Player> newPlayer, int id) {
	_playersMap[id] = newPlayer;
}

std::shared_ptr<Player> Game::GetPlayer(int id) {
	if (_playersMap.find(id) != _playersMap.end()) {
		return _playersMap[id];
	}

	return nullptr;
}

void Game::DeletePlayer(int id) {
	_playersMap.erase(id);
}

void Game::DeleteRoom(int id) {
	_roomsMap[id]->DeleteAllPlayersInRoom();
	_roomsMap.erase(id);
}

void Game::DeleteAllPlayers() {
	printf("Deleting all players\n");
	for (auto it = _playersMap.begin(); it != _playersMap.end(); it++)
		it->second->CloseSocket();

	_playersMap.clear();
	_roomsMap.clear();

	printf("All players deleted\n");
}

void Game::SetServer(std::shared_ptr<Handler> server) {
	_server = server;
}

void Game::EndGame() {
	DeleteAllPlayers();
	_server->Close();
}

bool Game::DoesRoomExist(int id) {
	for (auto it = _roomsMap.begin(); it != _roomsMap.end(); it++) {
		if (it->first == id)
			return true;
	}

	return false;
}

#include "Game.h"
#include <fstream>
#include <random>
#include <sstream>

void Game::Setup() {
	std::fstream wordsFile;
	wordsFile.open("Files//words.txt", std::ios::in);

	if (wordsFile.is_open()) {
		std::string word;
		while (wordsFile) {
			std::getline(wordsFile, word);
			if (word != "")
				words.emplace_back(word);
		}
		printf("Loaded passwords\n");
	}
	else {
		printf("Passwords couldn't be loaded\n");
		words.emplace_back("sieci");
	}

	std::fstream configFile;
	configFile.open("Files//config.txt", std::ios::in);
	if (configFile.is_open()) {
		std::string word;
		configFile >> word;
		_port = LoadNumber(word);

		if (_port == -1) {
			_port = 12345;
			printf("Error in loading port\n");
		}

		configFile >> word;
		_gameTime = LoadNumber(word);
		
		if (_gameTime == -1) {
			_gameTime = 120;
			printf("Error in loading game time\n");
		}

		configFile >> word;
		_waitingTime = LoadNumber(word);

		if (_waitingTime == -1) {
			_waitingTime = 30;
			printf("Error in loading waiting time");
		}

		printf("Loaded config file\n");
	}
	else {
		printf("Config file couldn't be loaded\n");
		_port = 12345;
	}
}

int Game::LoadNumber(std::string word) {
	try {
		std::stringstream ss(word);
		int number;

		if ((ss >> number).fail()) {
			throw std::bad_cast();
		}

		return number;
	}
	catch (std::bad_cast const&) {
		return -1;
	}
}

std::string Game::GetRandomWord() {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution<std::mt19937::result_type> dist(0, words.size() - 1);
	return words[dist(rng)];
}

void Game::SetServer(std::shared_ptr<Handler> server) {
	_server = server;
}

int Game::GetFreeRoomId() {
	for (int i = LOWER_BOUND_ROOM; i <= UPPER_BOUND_ROOM; i++) {
		if (_roomsMap.find(i) == _roomsMap.end())
			return i;
	}

	return -1;
}

int Game::GetFreePlayerId() {
	for (int i = LOWER_BOUND_PLAYER; i <= UPPER_BOUND_PLAYER; i++) {
		if (_playersMap.find(i) == _playersMap.end())
			return i;
	}

	return -1;
}

int Game::GetPort() {
	return _port;
}

int Game::GetGameTime() {
	return _gameTime;
}

int Game::GetWaitingTime() {
	return _waitingTime;
}

void Game::AddRoom(int id, int epollFd) {
	_roomsMap[id] = std::make_shared<Room>(id, epollFd);
}

std::shared_ptr<Room> Game::GetRoom(int id) {
	if (_roomsMap.find(id) != _roomsMap.end())
		return _roomsMap[id];

	return nullptr;
}

void Game::AddPlayer(std::shared_ptr<Player> newPlayer, int id) {
	_playersMap[id] = newPlayer;
}

std::shared_ptr<Player> Game::GetPlayer(int id) {
	if (_playersMap.find(id) != _playersMap.end())
		return _playersMap[id];

	return nullptr;
}

void Game::DeletePlayer(int id) {
	_playersMap.erase(id);
}

void Game::DeletePlayerFromRoom(int roomId, std::string name) {
	if(_roomsMap.find(roomId) != _roomsMap.end())
		_roomsMap[roomId]->DeletePlayer(name);
}

void Game::DeleteRoom(int id) {
	if (_roomsMap.find(id) != _roomsMap.end()) {
		_roomsMap[id]->Close();
		_roomsMap[id]->DeleteAllPlayersInRoom();
		_roomsMap.erase(id);
	}
}

void Game::DeleteAll() {
	printf("Deleting all players and rooms\n");
	for (auto it = _roomsMap.begin(); it != _roomsMap.end(); it++)
		it->second->Close();

	for (auto it = _playersMap.begin(); it != _playersMap.end(); it++)
		it->second->Close();

	_playersMap.clear();
	_roomsMap.clear();

	printf("All players and rooms deleted\n");
}

void Game::EndGame() {
	DeleteAll();
	_server->Close();
}

bool Game::DoesRoomExist(int roomId) {
	if(_roomsMap.find(roomId) != _roomsMap.end())
		return true;

	return false;
}

void Game::DeleteRoomAfterGame(int roomId) {
	if (_roomsMap.find(roomId) != _roomsMap.end()) {
		_roomsMap[roomId]->Close();
		_roomsMap[roomId]->ClearRoom();
		_roomsMap.erase(roomId);
	}
}

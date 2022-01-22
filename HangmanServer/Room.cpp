#include "Room.h"
#include "Game.h"
#include <thread>
#include <chrono>

Room::Room(int id, int epollFd) : _roomId(id), _epollFd(epollFd) { 
	_secretWord = Game::Instance().GetRandomWord();
	_timerRegistered = false;
	_timerCreated = false;
	_gameStarted = false;
}

void Room::Close() {
	if (_timerCreated) {
		close(_timerFd);
		printf("Closed timer fd\n");
		_timerCreated = false;
	}

	if (_timerRegistered) {
		epoll_ctl(_epollFd, EPOLL_CTL_DEL, _timerFd, nullptr);
		printf("Closed room timer fd\n");
		_timerRegistered = false;
	}

	printf("Room closed\n");
}

std::tuple<HandleResult, int, int, std::string> Room::Handle(uint events) {
	if (events & EPOLLIN) {
		uint64_t buff;
		read(_timerFd, &buff, sizeof(uint64_t));
		if (this->GetNumberOfPlayers() >= 2) {
			std::string message;
			message += (uint8_t)OperationCodes::StartGame;
			message += " Game started";
			SendToAll(message);
			_gameStarted = true;
		}
	}

	if (events & ~EPOLLIN) {
		Close();
		return std::make_tuple(HandleResult::DeleteRoom, _roomId, 0, "");
	}

	return std::make_tuple(HandleResult::NoHandleResError, 0, 0, "");
}

void Room::AddPlayer(std::shared_ptr<Player> player, std::string name) {
	_playersInRoom[name] = player;
}

std::shared_ptr<Player> Room::GetPlayer(const std::string& name){
	if (_playersInRoom.find(name) != _playersInRoom.end())
		return _playersInRoom[name];

	return nullptr;
}

void Room::DeletePlayer(std::string name) {
	_playersInRoom.erase(name);
}

void Room::DeleteAllPlayersInRoom() {
	for (auto it = _playersInRoom.begin(); it != _playersInRoom.end(); it++) {
		it->second->Close();
		Game::Instance().DeletePlayer(it->second->GetId());
	}

	_playersInRoom.clear();
}

void Room::SendToAllBut(std::string message, const std::string& name) {
	for (auto it = _playersInRoom.begin(); it != _playersInRoom.end(); it++) {
		if(it->first != name)
			it->second->PrepereToSend(message);
	}
}

bool Room::IsNameUnique(const std::string& name) {
	if (_playersInRoom.find(name) != _playersInRoom.end())
		return false;

	return true;
}

std::string Room::GetSecretWord() {
	return _secretWord;
}

std::string Room::GetAllPlayerNamesBut(const std::string& name) {
	std::string all = "";

	for (auto it = _playersInRoom.begin(); it != _playersInRoom.end(); it++) {
		if (it->first != name) {
			all += it->second->GetName();
			all += " ";
		}
	}

	return all;
}

bool Room::IsLetterInWord(char letter) {
	if (_secretWord.find(letter) != std::string::npos)
		return true;
	return false;
}

void Room::InsertCorrectLetter(char letter, std::string& word) {
	for (size_t i = 0; i < _secretWord.size(); i++) {
		if (_secretWord[i] == letter) {
			word[i] = letter;
		}
	}
}

void Room::SendToAll(std::string message) {
	for (auto it = _playersInRoom.begin(); it != _playersInRoom.end(); it++) {
		it->second->PrepereToSend(message);
	}
}

ParseMessegeError Room::StartTimer(int time) {
	if (!_timerCreated) {
		_timerFd = timerfd_create(CLOCK_REALTIME, 0);

		if (_timerFd == -1)
			return ParseMessegeError::TimerFailed;
	}

	struct itimerspec ts;
	ts.it_value.tv_sec = time;
	ts.it_value.tv_nsec = 0;
	ts.it_interval.tv_sec = 0;
	ts.it_interval.tv_nsec = 0;

	if (timerfd_settime(_timerFd, 0, &ts, nullptr) == -1)
		return ParseMessegeError::TimerFailed;

	epoll_event ee{ EPOLLIN | EPOLLRDHUP, {.ptr = this} };
	epoll_ctl(_epollFd, EPOLL_CTL_ADD, _timerFd, &ee);
	_timerRegistered = true;

	return ParseMessegeError::NoMsgError;
}

void Room::ResetTimer() {
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, _timerFd, nullptr);
	_timerRegistered = false;
	std::string message;
	message += (uint8_t)OperationCodes::TimerStopped;
	SendToAll(message);
}

bool Room::GetGameStarted() {
	return _gameStarted;
}

int Room::GetNumberOfPlayers() {
	return _playersInRoom.size();
}

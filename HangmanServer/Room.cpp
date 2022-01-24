#include "Room.h"
#include "Game.h"
#include <chrono>

Room::Room(int id, int epollFd) : _roomId(id), _epollFd(epollFd) { 
	_secretWord = Game::Instance().GetRandomWord();
	_timerRegistered = false;
	_timerCreated = false;
	_gameStarted = false;
	_gameTime = 120;
}

void Room::Close() {
	if (_timerCreated) {
		close(_timerFd);
		printf("Closed timer fd\n");
		_timerCreated = false;
	}

	if (_timerRegistered) {
		epoll_ctl(_epollFd, EPOLL_CTL_DEL, _timerFd, nullptr);
		printf("Timer unregistered\n");
		_timerRegistered = false;
	}

	printf("Room closed\n");
}

std::tuple<HandleResult, int, int, std::string> Room::Handle(uint events) {
	bool setTimer = false;

	if (events & EPOLLIN) {
		uint64_t buff;
		ssize_t count = read(_timerFd, &buff, sizeof(uint64_t));
		
		if (count <= 0) {
			events |= EPOLLERR;
		}
		else if (this->GetNumberOfPlayers() >= 2 && !_gameStarted) {
			std::string message;
			message += (uint8_t)OperationCodes::StartGame;
			message += " ";
			message += GetHiddenSecretWord();
			SendToAll(message);
			_gameStarted = true;
			setTimer = true;
		}
		else if (_gameStarted) {
			SendWinner();
			return std::make_tuple(HandleResult::EndGameInRoom, _roomId, 0, "");
		}
	}

	if (events & ~EPOLLIN) {
		Close();
		return std::make_tuple(HandleResult::DeleteRoom, _roomId, 0, "");
	}

	if (setTimer) {
		StartTimer(_gameTime);
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

bool Room::IsLetterInSecretWord(char letter) {
	if (_secretWord.find(letter) != std::string::npos)
		return true;
	return false;
}

void Room::InsertCorrectLetter(char letter, std::string& word) {
	for (int i = 0; i < _secretWord.size(); i++) {
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

ParseMessegeStatus Room::StartTimer(int time) {
	if (!_timerCreated) {
		_timerFd = timerfd_create(CLOCK_REALTIME, 0);

		if (_timerFd == -1)
			return ParseMessegeStatus::TimerFailed;
		_timerCreated = true;
	}

	struct itimerspec ts;
	ts.it_value.tv_sec = time;
	ts.it_value.tv_nsec = 0;
	ts.it_interval.tv_sec = 0;
	ts.it_interval.tv_nsec = 0;

	if (timerfd_settime(_timerFd, 0, &ts, nullptr) == -1)
		return ParseMessegeStatus::TimerFailed;

	epoll_event ee{ EPOLLIN | EPOLLRDHUP, {.ptr = this} };
	epoll_ctl(_epollFd, EPOLL_CTL_ADD, _timerFd, &ee);
	_timerRegistered = true;

	return ParseMessegeStatus::NoMsgError;
}

void Room::ResetTimer() {
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, _timerFd, nullptr);
	_timerRegistered = false;
	std::string message;
	message += (uint8_t)OperationCodes::TimerStopped;
	message += " StoppedWaiting";
	SendToAll(message);
}

bool Room::GameStarted() {
	return _gameStarted;
}

std::string Room::GetHiddenSecretWord() {
	std::string word = "";
	printf("%s\n", _secretWord.c_str());
	for (int i = 0; i < _secretWord.size(); i++) {
		if (_secretWord[i] != ' ' && _secretWord[i] != '\n' && _secretWord[i] != '\r') {
			word += "*";
		}
		else if(_secretWord[i] != '\n' && _secretWord[i] != '\r') {
			word += "_";
		}
	}

	return word;
}

std::string Room::GetWinnerPlayer() {
	int max = -1;
	std::string name;
	for (auto it = _playersInRoom.begin(); it != _playersInRoom.end(); it++) {
		if (it->second->GetPoints() > max && it->second->GetHangmanState() < Player::MAX_HANGMAN) {
			max = it->second->GetPoints();
			name = it->first;
		}
		else if (max != -1 && it->second->GetPoints() == max && it->second->GetHangmanState() < Player::MAX_HANGMAN) {
			name = "Draw";
		}

		printf("%s %d\n", it->first.c_str(), it->second->GetPoints());
	}

	if (max == -1) {
		name = "Draw";
	}

	name += " ";
	name += _secretWord;

	return name;
}

void Room::ClearRoom() {
	for (auto it = _playersInRoom.begin(); it != _playersInRoom.end(); it++) {
		it->second->SetRoomId(-1);
		it->second->SetName("");
	}

	_playersInRoom.clear();
}

void Room::SendWinner() {
	std::string message;
	std::string winner = GetWinnerPlayer();
	message += (uint8_t)OperationCodes::EndGame;
	message += " ";
	message += winner;
	SendToAll(message);
}

bool Room::EveryoneHasHangman() {
	int number = 0;
	for (auto it = _playersInRoom.begin(); it != _playersInRoom.end(); it++) {
		if (it->second->GetHangmanState() == Player::MAX_HANGMAN)
			number++;
	}

	if (number == _playersInRoom.size())
		return true;
	return false;
}

bool Room::WinnerFound() {
	for (auto it = _playersInRoom.begin(); it != _playersInRoom.end(); it++) {
		std::string playerWord = it->second->GetCurrentWord();
		int foundStars = 0;
		for (int i = 0; i < playerWord.size(); i++) {
			if (playerWord[i] == '*')
				foundStars++;
		}

		if (foundStars == 0 && it->second->GetHangmanState() != Player::MAX_HANGMAN)
			return true;
	}

	return false;
}

int Room::GetNumberOfPlayers() {
	return _playersInRoom.size();
}

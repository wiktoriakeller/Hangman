#include "Player.h"
#include "Game.h"

Player::Player(int socket, int id, int epollFd) : _socket(socket), _id(id), _roomId(-1), _epollFd(epollFd), _hangmanState(0), _closedSocket(false) {
	epoll_event ee{ EPOLLIN | EPOLLRDHUP, {.ptr = this} };
	epoll_ctl(_epollFd, EPOLL_CTL_ADD, _socket, &ee);
}

void Player::Close() {
	if (!_closedSocket) {
		_closedSocket = true;
		epoll_ctl(_epollFd, EPOLL_CTL_DEL, _socket, nullptr);
		shutdown(_socket, SHUT_RDWR);
		close(_socket);
		printf("Player socket closed\n");
	}
}

int Player::GetId() {
	return _id;
}

void Player::SetName(const std::string& name) {
	_name = name;
}

std::string Player::GetName() {
	return _name;
}

void Player::SetRoomId(int id) {
	_roomId = id;
}

std::tuple<HandleResult, int, int, std::string> Player::Handle(uint events) {
	bool timerError = false;
	bool everyoneHasHangman = false;
	bool winnerFound = false;

	if (events & EPOLLIN) {
		char buffer[BUFFER_SIZE];
		memset(buffer, 0x00, BUFFER_SIZE);
		ssize_t count = read(_socket, buffer, BUFFER_SIZE - 1);
		buffer[count] = '\0';

		if (count <= 0) {
			events |= EPOLLERR;
		}
		else {
			currentMessagesToRead.insert(currentMessagesToRead.end(), buffer, buffer + count);
			std::string tmpMessage;

			for (int i = 0; i < currentMessagesToRead.size(); i++) {
				if (currentMessagesToRead[i] == '\n') {
					tmpMessage += currentMessagesToRead[i];
					ParseMessegeStatus error = ParseMessage(tmpMessage);

					if (error == ParseMessegeStatus::TimerFailed)
						timerError = true;
					else if (error == ParseMessegeStatus::EveryoneHasHangman)
						everyoneHasHangman = true;
					else if (error == ParseMessegeStatus::WinnerFound)
						winnerFound = true;

					tmpMessage.clear();
				}
				else {
					tmpMessage += currentMessagesToRead[i];
				}
			}

			currentMessagesToRead.clear();

			if (tmpMessage.size() != 0) {
				currentMessagesToRead.assign(tmpMessage.begin(), tmpMessage.end());
			}
		}
	}

	if (events & EPOLLOUT) {
		int i = 0;
		bool dataToSend = currentMessagesToSend.size() > 0 ? true : false;

		while (dataToSend) {
			const char* data = currentMessagesToSend[i].c_str();
			size_t size = currentMessagesToSend[i].size();
			ssize_t sent = send(_socket, data, size, MSG_DONTWAIT);

			if (sent == -1) {
				if (errno != EWOULDBLOCK && errno != EAGAIN) {
					events |= EPOLLERR;
				}

				break;
			}
			else if (sent != size) {
				currentMessagesToSend[i] = currentMessagesToSend[i].substr(sent);
			}
			else {
				i++;
			}

			if (i == currentMessagesToSend.size())
				dataToSend = false;
		}

		if (i == currentMessagesToSend.size()) {
			WaitForWrite(false);
			currentMessagesToSend.clear();
		}
		else {
			currentMessagesToSend.erase(std::next(currentMessagesToSend.begin(), 0), std::next(currentMessagesToSend.begin(), i));
		}
	}

	if (winnerFound) {
		printf("Winner found\n");
		std::shared_ptr<Room> room = Game::Instance().GetRoom(_roomId);
		room->SendWinner();
		return std::make_tuple(HandleResult::EndGameInRoom, _roomId, 0, "");
	}

	if (everyoneHasHangman) {
		printf("Everyone has hangman\n");
		std::shared_ptr<Room> room = Game::Instance().GetRoom(_roomId);
		room->SendWinner();
		return std::make_tuple(HandleResult::EndGameInRoom, _roomId, 0, "");
	}

	if (timerError) {
		return std::make_tuple(HandleResult::DeleteRoom, _roomId, 0, "");
	}

	if (events & ~(EPOLLIN | EPOLLOUT)) {
		Close();
		return std::make_tuple(HandleResult::DeletePlayer, _id, _roomId, _name);
	}

	return std::make_tuple(HandleResult::NoHandleResError, 0, 0, "");
}

void Player::WaitForWrite(bool epollout) {
	if (epollout) {
		epoll_event ee{ { EPOLLIN | EPOLLOUT | EPOLLRDHUP }, {.ptr = this } };
		epoll_ctl(_epollFd, EPOLL_CTL_MOD, _socket, &ee);
	}
	else {
		epoll_event ee{ { EPOLLIN | EPOLLRDHUP }, {.ptr = this} };
		epoll_ctl(_epollFd, EPOLL_CTL_MOD, _socket, &ee);
	}
}

void Player::PrepereToSend(std::string message) {
	if (message.size() > 0) {
		message += "\n";
		WaitForWrite(true);
		currentMessagesToSend.emplace_back(message);
	}
}

int Player::GetPoints() {
	return _points;
}

int Player::GetHangmanState() {
	return _hangmanState;
}

std::string Player::GetCurrentWord() {
	return _currentWord;
}

ParseMessegeStatus Player::HandleOperation(const std::vector<std::string>& divided) {
	//OperationCodes operationType = (OperationCodes)divided[0][0];
	OperationCodes operationType = (OperationCodes)stoi(divided[0] + '\0');
	ParseMessegeStatus error;

	switch (operationType)
	{
	case OperationCodes::SendNewRoomId:
		error = SendNewRoomId(divided);
		break;
	case OperationCodes::JoinRoom:
		error = JoinRoom(divided);
		break;
	case OperationCodes::SendWord:
		error = SendWord();
		break;
	case OperationCodes::CheckLetter:
		error = CheckLetter(divided[1][0]);
		break;
	default:
		break;
	}

	return error;
}

ParseMessegeStatus Player::ParseMessage(std::string message) {
	std::vector<std::string> divided;
	std::string messagePart;

	for (int i = 0; i < message.size(); i++) {
		if (message[i] == '\n' && messagePart.size() != 0) {
			divided.emplace_back(messagePart);
		}
		else if (message[i] == ' ') {
			divided.emplace_back(messagePart);
			messagePart.clear();
		}
		else {
			messagePart += message[i];
		}
	}

	ParseMessegeStatus error = HandleOperation(divided);
	return error;
}

ParseMessegeStatus Player::SendNewRoomId(const std::vector<std::string>& divided) {
	std::string toSend;
	std::string name = divided[1];
	int generatedRoomId = Game::Instance().GetFreeRoomId();
	if (generatedRoomId == -1) {
		toSend += (uint8_t)OperationCodes::RoomCreationFailed;
	}
	else {
		SetName(name);
		SetRoomId(generatedRoomId);
		Game::Instance().AddRoom(generatedRoomId, _epollFd);
		std::shared_ptr<Room> room = Game::Instance().GetRoom(generatedRoomId);
		room->AddPlayer(Game::Instance().GetPlayer(_id), name);
		_currentWord = room->GetHiddenSecretWord();
		_points = 0;

		toSend += (uint8_t)OperationCodes::SendNewRoomId;
		toSend += " ";
		toSend += std::to_string(generatedRoomId);
	}

	PrepereToSend(toSend);
	return ParseMessegeStatus::NoMsgError;
}

ParseMessegeStatus Player::JoinRoom(const std::vector<std::string>& divided) {
	std::string toSend;
	int roomId = stoi(divided[1]);
	std::string name = divided[2];
	ParseMessegeStatus error = ParseMessegeStatus::NoMsgError;
	
	if (!Game::Instance().DoesRoomExist(roomId)) {
		toSend += (uint8_t)OperationCodes::InvalidRoom;
		PrepereToSend(toSend);
		return ParseMessegeStatus::NoMsgError;
	}
	
	std::shared_ptr<Room> room = Game::Instance().GetRoom(roomId);

	if (!room->IsNameUnique(name)) {
		toSend += (uint8_t)OperationCodes::NotUniqueName;
		PrepereToSend(toSend);
		return ParseMessegeStatus::NoMsgError;
	}

	if (room->GetNumberOfPlayers() == Room::ROOM_MAX_SIZE) {
		toSend += (uint8_t)OperationCodes::RoomIsFull;
		PrepereToSend(toSend);
		return ParseMessegeStatus::NoMsgError;
	}

	if (room->GameStarted()) {
		toSend += (uint8_t)OperationCodes::GameAlreadyStarted;
		PrepereToSend(toSend);
		return ParseMessegeStatus::NoMsgError;
	}

	SetName(name);
	SetRoomId(roomId);
	_points = 0;
	_currentWord = room->GetHiddenSecretWord();

	room->AddPlayer(Game::Instance().GetPlayer(_id), name);
	toSend += (uint8_t)OperationCodes::JoinRoom;
	toSend += " ";
	toSend += room->GetAllPlayerNamesBut(name);

	std::string messageToAll;
	messageToAll += (uint8_t)OperationCodes::SendPlayerName;
	messageToAll += " ";
	messageToAll += name;

	room->SendToAllBut(messageToAll, name);
	PrepereToSend(toSend);

	if (room->GetNumberOfPlayers() == 2) {
		std::string message;
		message += (uint8_t)OperationCodes::StartedWaiting;
		message += " StartedWaiting";
		room->SendToAll(message);
		error = room->StartTimer(25);
	}

	return error;
}

ParseMessegeStatus Player::SendWord() {
	std::string toSend;

	if (_roomId < 0) {
		toSend += (uint8_t)OperationCodes::NotInARoom;
		PrepereToSend(toSend);
		return ParseMessegeStatus::NoMsgError;
	}

	toSend += (uint8_t)OperationCodes::SendWord;
	toSend += " ";
	toSend += _currentWord;
	PrepereToSend(toSend);
	return ParseMessegeStatus::NoMsgError;
}

ParseMessegeStatus Player::CheckLetter(char letter) {
	std::string toSend;
	if (_roomId < 0) {
		toSend += (uint8_t)OperationCodes::NotInARoom;
		PrepereToSend(toSend);
		return ParseMessegeStatus::NoMsgError;
	}

	std::shared_ptr<Room> room = Game::Instance().GetRoom(_roomId);
	
	if (room->IsLetterInSecretWord(letter)) {
		if (_currentWord.find(letter) == std::string::npos) {
			_points++;
		}

		room->InsertCorrectLetter(letter, _currentWord);
		toSend += (uint8_t)OperationCodes::CorrectLetter;
		toSend += " ";
		toSend += _currentWord;

		PrepereToSend(toSend);


		bool status = room->WinnerFound();
		if (status)
			return ParseMessegeStatus::WinnerFound;
	}
	else {
		if (_hangmanState < Player::MAX_HANGMAN)
			_hangmanState++;

		toSend += (uint8_t)OperationCodes::IncorrectLetter;
		toSend += " ";
		toSend += std::to_string(_hangmanState);
		PrepereToSend(toSend);

		std::string messageToAll;
		messageToAll += (uint8_t)OperationCodes::SendHangmanWithName;
		messageToAll += " ";
		messageToAll += _name;
		messageToAll += " ";
		messageToAll += std::to_string(_hangmanState);
		room->SendToAllBut(messageToAll, _name);

		bool status = room->EveryoneHasHangman();
		if (status)
			return ParseMessegeStatus::EveryoneHasHangman;
	}

	return ParseMessegeStatus::NoMsgError;
}

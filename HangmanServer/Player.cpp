#include "Player.h"
#include "Game.h"

Player::Player(int socket, int id, int epollFd) {
	_socket = socket;
	_id = id;
	_roomId = -1;
	_epollFd = epollFd;
	epoll_event ee{ EPOLLIN | EPOLLRDHUP, {.ptr = this} };
	epoll_ctl(_epollFd, EPOLL_CTL_ADD, _socket, &ee);
}

void Player::Close() {
	CloseSocket();
	Game::Instance().DeletePlayer(_id);
	std::shared_ptr<Room> room = Game::Instance().GetRoom(_roomId);
	if(room != nullptr)
		room->DeletePlayer(_name);
	printf("Player closed\n");
}

void Player::CloseSocket() {
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, _socket, nullptr);
	shutdown(_socket, SHUT_RDWR);
	close(_socket);
	printf("Player socket closed\n");
}

void Player::SetId(int id) {
	_id = id;
}

int Player::GetId() {
	return _id;
}

void Player::SetName(std::string name) {
	_name = name;
}

std::string Player::GetName() {
	return _name;
}

void Player::SetRoomId(int id) {
	_roomId = id;
}

int Player::GetRoomId() {
	return _roomId;
}

HandleResult Player::Handle(uint events) {
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

			for (long unsigned int i = 0; i < currentMessagesToRead.size(); i++) {
				if (currentMessagesToRead[i] == '\n') {
					tmpMessage += currentMessagesToRead[i];
					ParseMessage(tmpMessage);
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
		bool dataToSend = currentMessagesToSend.size() > 0 ? true : false;
		while (dataToSend) {
			int i = currentMessagesToSend.size() - 1;
			const char* data = currentMessagesToSend[i].c_str();
			size_t size = currentMessagesToSend[i].size();
			int sent = send(_socket, data, size, MSG_DONTWAIT);

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
				currentMessagesToSend.pop_back();
			}

			if (currentMessagesToSend.size() == 0)
				dataToSend = false;
		}

		if (currentMessagesToSend.size() == 0)
			WaitForWrite(false);
	}

	if (events & ~(EPOLLIN | EPOLLOUT)) {
		Close();
		return HandleResult::DeletePlayer;
	}

	return HandleResult::None;
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

void Player::PrepereToSend(std::vector<std::string>& messageParts) {
	std::string fullMessage;

	for (long unsigned int i = 0; i < messageParts.size(); i++) {
		fullMessage += messageParts[i];
		fullMessage += " ";
	}

	fullMessage += "\n";

	WaitForWrite(true);
	currentMessagesToSend.emplace_back(fullMessage);
}

void Player::HandleOperation(std::vector<std::string>& splitted) {
	OperationCodes operationType = (OperationCodes)splitted[0][0];
	std::vector<std::string> toSend;

	switch (operationType)
	{
	case OperationCodes::SendNewRoomId:
		toSend.emplace_back("message sent!\0");
	default:
		break;
	}

	if (toSend.size() != 0)
		PrepereToSend(toSend);
}

void Player::ParseMessage(std::string message) {
	std::vector<std::string> splitted;
	std::string messagePart;

	for (long unsigned int i = 0; i < message.size(); i++) {
		if (message[i] == '\n' && messagePart.size() != 0) {
			splitted.emplace_back(messagePart);
		}
		else if (message[i] == ' ') {
			splitted.emplace_back(messagePart);
			messagePart.clear();
		}
		else {
			messagePart += message[i];
		}
	}

	HandleOperation(splitted);
}

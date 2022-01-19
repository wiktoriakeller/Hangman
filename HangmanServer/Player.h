#pragma once

#include "Handler.h"

class Player : public Handler {
public:
	Player(int socket);
	~Player();
	void Handle(uint events) override;

private:
	const size_t BUFFER_SIZE = 8;
	int _socket;
	std::vector<char> currentMessagesToRead;
	std::vector<std::string> currentMessagesToSend;

	void WaitForWrite(bool epollout);
	void PrepereToSend(std::vector<std::string>& messageParts);
	void HandleOperation(std::vector<std::string>& splitted);
	void ParseMessage(std::string message);
};

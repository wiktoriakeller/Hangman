#pragma once

#include "Handler.h"

class Player : public Handler {
public:
	Player(int socket, int id, int epollFd);
	void Close() override;
	HandleResult Handle(uint events) override;
	void SetId(int id);
	int GetId();
	void SetName(std::string name);
	std::string GetName();
	void SetRoomId(int id);
	int GetRoomId();
	void CloseSocket();

private:
	const size_t BUFFER_SIZE = 8;
	int _socket;
	int _id;
	int _roomId;
	int _epollFd;
	std::string _name;
	std::vector<char> currentMessagesToRead;
	std::vector<std::string> currentMessagesToSend;

	void WaitForWrite(bool epollout);
	void PrepereToSend(std::vector<std::string>& messageParts);
	void HandleOperation(std::vector<std::string>& splitted);
	void ParseMessage(std::string message);	
};

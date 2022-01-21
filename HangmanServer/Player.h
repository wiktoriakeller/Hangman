#pragma once

#include "Handler.h"

class Player : public Handler {
public:
	Player(int socket, int id, int epollFd);
	void Close() override;
	HandleResult Handle(uint events) override;
	void SetId(const int& id);
	int GetId();
	void SetName(const std::string& name);
	std::string GetName();
	void SetRoomId(int id);
	int GetRoomId();
	void CloseSocket();
	void PrepereToSend(std::string message);

private:
	const size_t BUFFER_SIZE = 64;
	int _socket;
	int _id;
	int _roomId;
	int _epollFd;
	std::string _name;
	std::vector<char> currentMessagesToRead;
	std::vector<std::string> currentMessagesToSend;

	void WaitForWrite(bool epollout);
	void HandleOperation(const std::vector<std::string>& divided);
	void ParseMessage(std::string message);	

	//operation codes handlers
	void SendNewRoomId(const std::vector<std::string>& divided);
	void JoinRoom(const std::vector<std::string>& divided);
	void SendWord();
};

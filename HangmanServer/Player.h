#pragma once
#include "Handler.h"

class Player : public Handler {
public:
	static const int MAX_HANGMAN = 6;
	Player(int socket, int id, int epollFd);
	void Close() override;
	std::tuple<HandleResult, int, int, std::string> Handle(uint events) override;
	int GetId();
	void SetName(const std::string& name);
	std::string GetName();
	void SetRoomId(int id);
	void PrepereToSend(std::string message);
	int GetPoints();
	int GetHangmanState();
	std::string GetCurrentWord();

private:
	const size_t BUFFER_SIZE = 128;
	int _socket;
	int _id;
	int _roomId;
	int _epollFd;
	int _hangmanState;
	bool _closedSocket;
	int _points;
	std::string _currentWord;
	std::string _name;
	std::vector<char> currentMessagesToRead;
	std::vector<std::string> currentMessagesToSend;

	void WaitForWrite(bool epollout);
	ParseMessegeStatus HandleOperation(const std::vector<std::string>& divided);
	ParseMessegeStatus ParseMessage(std::string message);	

	//operation codes handlers
	ParseMessegeStatus SendNewRoomId(const std::vector<std::string>& divided);
	ParseMessegeStatus JoinRoom(const std::vector<std::string>& divided);
	ParseMessegeStatus CheckLetter(char letter);
};

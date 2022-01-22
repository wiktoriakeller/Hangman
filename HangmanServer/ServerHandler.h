#pragma once
#include "Handler.h"

class ServerHandler : public Handler {
public:
	ServerHandler(int epollFd);
	void Close() override;
	std::tuple<HandleResult, int, int, std::string> Handle(uint events) override;
	int GetServerSocket();

private:
	int _serverSocket;
	int _epollFd;
	const int SERVER_PORT = 12345;
	const int one = 1;
};

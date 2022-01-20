#pragma once
#include "Handler.h"

class ServerHandler : public Handler {
public:
	ServerHandler();
	void Close() override;
	HandleResult Handle(uint events) override;
	void SetEpollFd(int epollFd);
	int GetServerSocket();

private:
	int _serverSocket;
	int _epollFd;
	const int SERVER_PORT = 12345;
	const int one = 1;
};

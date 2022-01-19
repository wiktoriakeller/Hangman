#pragma once
#include "Handler.h"

class ServerHandler : public Handler {
public:
	ServerHandler();
	~ServerHandler();
	void Handle(uint events) override;

private:
	int _serverSocket;
	const int SERVER_PORT = 12345;
	const int one = 1;
};

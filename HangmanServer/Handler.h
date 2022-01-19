#pragma once
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <error.h>
#include <thread>
#include <string>
#include <sys/epoll.h>
#include <signal.h>
#include <map>
#include <vector>

enum OperationCodes {
	SendNewRoomId
};

enum HandleResult {
	None,
	DeletePlayer,
	DeleteServer
};

class Handler {
public:
	virtual void Close() = 0;
	virtual HandleResult Handle(uint events) = 0;
};

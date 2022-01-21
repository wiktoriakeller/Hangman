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
	SendNewRoomId = 1,
	JoinRoom = 2,
	NotUniqueName = 3,
	InvalidRoom = 4,
	GameAlreadyStarted = 5,
	SendPlayerName = 6,
	SendAllPlayerNames = 7,
	RoomIsFull = 8,
	SendWord = 9,
	CheckLetter = 11,
	IncorrectLetter = 12,
	CorrectLetter = 13,
	PlayerLeft = 14,
	EndGame = 15
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

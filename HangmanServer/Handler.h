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
#include <tuple>

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
	SendHangmanWithName = 14,
	EndGame = 16,
	StartGame = 18,
	StartedWaiting = 19,
	TimerStopped = 20,
	RoomCreationFailed = 21
};

enum HandleResult {
	NoHandleResError,
	DeletePlayer,
	DeleteRoom,
	DeleteServer,
	EndGameInRoom
};

enum ParseMessegeStatus {
	NoMsgError,
	TimerFailed,
	EveryoneHasHangman
};

class Handler {
public:
	virtual void Close() = 0;
	virtual std::tuple<HandleResult, int, int, std::string> Handle(uint events) = 0;
};

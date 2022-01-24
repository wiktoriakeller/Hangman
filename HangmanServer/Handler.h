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
	SendNewRoomId = 48,
	JoinRoom = 49,
	NotUniqueName = 50,
	InvalidRoom = 51,
	GameAlreadyStarted = 52,
	SendPlayerName = 53,
	SendAllPlayerNames = 54,
	RoomIsFull = 55,
	CheckLetter = 57,
	IncorrectLetter = 58,
	CorrectLetter = 59,
	SendHangmanWithName = 60,
	EndGame = 61,
	StartGame = 62,
	StartedWaiting = 63,
	TimerStopped = 64,
	RoomCreationFailed = 65,
	NotInARoom = 66,
	PlayerLeft = 67
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
	EveryoneHasHangman,
	WinnerFound
};

class Handler {
public:
	virtual void Close() = 0;
	virtual std::tuple<HandleResult, int, int, std::string> Handle(uint events) = 0;
};

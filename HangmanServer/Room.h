#pragma once
#include "Player.h"
#include <memory>
#include <map>
#include <sys/timerfd.h>

class Room : public Handler {
public:
	static const int ROOM_MAX_SIZE = 6;
	Room(int id, int epollFd);
	void Close() override;
	std::tuple<HandleResult, int, int, std::string> Handle(uint events) override;
	void AddPlayer(std::shared_ptr<Player> player, std::string name);
	std::shared_ptr<Player> GetPlayer(const std::string& name);
	void DeletePlayer(std::string name);
	void DeleteAllPlayersInRoom();
	void SendToAllBut(std::string message, const std::string& name);
	bool IsNameUnique(const std::string& name);
	int GetNumberOfPlayers();
	std::string GetSecretWord();
	std::string GetAllPlayerNamesBut(const std::string& name);
	bool IsLetterInWord(char letter);
	void InsertCorrectLetter(char letter, std::string& word);
	void SendToAll(std::string message);
	ParseMessegeError StartTimer(int time);
	void ResetTimer();
	bool GetGameStarted();

private:
	int _roomId;
	int _timerFd;
	bool _timerRegistered;
	bool _timerCreated;
	int _epollFd;
	bool _gameStarted;
	std::string _secretWord;
	std::map<std::string, std::shared_ptr<Player>> _playersInRoom;
};

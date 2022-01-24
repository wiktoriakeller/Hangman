#include "ServerHandler.h"
#include "Player.h"
#include "Game.h"
#include <fstream>

ServerHandler::ServerHandler(int epollFd) : _epollFd(epollFd) {
	sockaddr_in serverAddr{};

	std::fstream configFile;
	configFile.open("Files//config.txt", std::ios::in);
	int port;
	if (configFile.is_open()) {
		std::string word;
		configFile >> word;
		port = stoi(word);
		printf("Loaded config file\n");
	}
	else {
		printf("Config file couldn't be loaded\n");
		port = 12345;
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	_serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (_serverSocket == -1)
		error(1, errno, "Socket creation failed");

	int res = setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

	if (res)
		error(1, errno, "Setsockopt failed");

	if (bind(_serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) != 0)
		error(1, errno, "Bind failed");

	if (listen(_serverSocket, 3))
		error(1, errno, "Listen failed");
}

void ServerHandler::Close() {
	shutdown(_serverSocket, SHUT_RDWR);
	close(_serverSocket);
	printf("Server closed\n");
}

std::tuple<HandleResult, int, int, std::string> ServerHandler::Handle(uint events) {
	if (events & EPOLLIN) {
		sockaddr_in clientAddr{};
		socklen_t clientAddrSize;
		clientAddrSize = sizeof(clientAddr);
		int clientSocket = accept(_serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

		if (clientSocket != -1) {
			int playerId = Game::Instance().GetFreePlayerId();
			if (playerId != -1) {
				std::shared_ptr<Player> player = std::make_shared<Player>(clientSocket, playerId, _epollFd);
				Game::Instance().AddPlayer(player, playerId);
				printf("Added new player\n");
			}
		}
	}

	if (events & ~EPOLLIN) {
		return std::make_tuple(HandleResult::DeleteServer, 0, 0, "");
	}

	return std::make_tuple(HandleResult::NoHandleResError, 0, 0, "");;
}

int ServerHandler::GetServerSocket() {
	return _serverSocket;
}

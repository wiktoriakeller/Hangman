#include "ServerHandler.h"
#include "Player.h"
#include "Game.h"

ServerHandler::ServerHandler(int epollFd) {
	_epollFd = epollFd;
	sockaddr_in serverAddr{};

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	_serverSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (_serverSocket == -1)
		error(1, errno, "Socket creation failed");

	setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

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

HandleResult ServerHandler::Handle(uint events) {
	if (events & EPOLLIN) {
		sockaddr_in clientAddr{};
		socklen_t clientAddrSize;
		clientAddrSize = sizeof(clientAddr);
		int clientSocket = accept(_serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

		int playerId = Game::Instance().GetFreePlayerId();
		std::shared_ptr<Player> player = std::make_shared<Player>(clientSocket, playerId, _epollFd);
		Game::Instance().AddPlayer(player, playerId);
		printf("Added new player\n");
	}

	if (events & ~EPOLLIN) {
		return HandleResult::DeleteServer;
	}

	return HandleResult::None;
}

int ServerHandler::GetServerSocket() {
	return _serverSocket;
}
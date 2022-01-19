#include "ServerHandler.h"
#include "Player.h"
#include "Game.h"

ServerHandler::ServerHandler() {
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
	
	Game::Instance().SetServerSocket(_serverSocket);
}

ServerHandler::~ServerHandler() {
	shutdown(_serverSocket, SHUT_RDWR);
	close(_serverSocket);
}

void ServerHandler::Handle(uint events) {
	if (events & EPOLLIN) {
		sockaddr_in clientAddr{};
		socklen_t clientAddrSize;
		clientAddrSize = sizeof(clientAddr);
		int clientSocket = accept(_serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

		Player* player = new Player(clientSocket);
	}
}

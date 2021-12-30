#include "HangmanServer.h"
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

using namespace std;

void queryNewClient(int clientSocket) {
	const int bufferSize = 256;
	char buffer[bufferSize];
	
	while (true) {
		write(clientSocket, "Room ID: ", 10);
		read(clientSocket, buffer, bufferSize);
		cout << buffer << "\n";

		if (!std::string(buffer).empty()) {
			cout << "Goodbye!" << "\n";
			break;
		}
	}

	shutdown(clientSocket, SHUT_RDWR);
	close(clientSocket);
}

int main() {	
	const int SERVER_PORT = 12345;
	const int one = 1;

	int clientSocket;
	int serverSocket;

	sockaddr_in serverAddr;
	sockaddr_in clientAddr;
	socklen_t clientAddrSize;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);

	if (serverSocket == -1)
		error(1, errno, "Socket creation failed");

	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

	if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) != 0)
		error(1, errno, "Bind failed");

	listen(serverSocket, 20);

	while (true) {
		clientAddrSize = sizeof(clientAddr);
		clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
		thread clientQueryThread(queryNewClient, clientSocket);
		clientQueryThread.detach();
	}
	
	shutdown(serverSocket, SHUT_RDWR);
	close(serverSocket);
}

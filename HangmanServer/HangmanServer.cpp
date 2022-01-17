#include "HangmanServer.h"

using namespace std;

int epollFd;
int serverSocket;
void ctrl_c(int);

class Handler {
public:
	virtual ~Handler() {};
	virtual void handle(uint events) = 0;
};

class ClientHandler : Handler {
private:
	int _socket;
public:
	ClientHandler(int socket) {
		_socket = socket;
		epoll_event ee{ EPOLLIN | EPOLLRDHUP, {.ptr = this} };
		epoll_ctl(epollFd, EPOLL_CTL_ADD, _socket, &ee);
	}
	~ClientHandler() {
		epoll_ctl(epollFd, EPOLL_CTL_DEL, _socket, nullptr);
		shutdown(_socket, SHUT_RDWR);
		close(_socket);
	}
	void handle(uint events) override {
		if (events & EPOLLIN) {
            printf("Hangle client\n");
            char buffer[64];
            ssize_t count = read(_socket, buffer, 64);
		}
		if (events & ~EPOLLIN) {
			delete this;
		}
	}
};

class ServerHandler : Handler {
public:
	ServerHandler() {
		sockaddr_in serverAddr{};

		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(SERVER_PORT);
		serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		serverSocket = socket(PF_INET, SOCK_STREAM, 0);
		if (serverSocket == -1)
			error(1, errno, "Socket creation failed");

		setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

		if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) != 0)
			error(1, errno, "Bind failed");

		if (listen(serverSocket, 3))
			error(1, errno, "Listen failed");
	}

	~ServerHandler() {
		shutdown(serverSocket, SHUT_RDWR);
		close(serverSocket);
	}

	void handle(uint events) override {
		if (events & EPOLLIN) {
			sockaddr_in clientAddr{};
			socklen_t clientAddrSize;
			clientAddrSize = sizeof(clientAddr);
			int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

			ClientHandler* client = new ClientHandler(clientSocket);
		}
	}
};


int main() {	
	
	ServerHandler* serverHandler = new ServerHandler();

	epollFd = epoll_create1(0);
	signal(SIGINT, ctrl_c);
	
	epoll_event epollEvent{ EPOLLIN, {.ptr = serverHandler} };
	epoll_ctl(epollFd, EPOLL_CTL_ADD, serverSocket, &epollEvent);

	while (true) {
		if (-1 == epoll_wait(epollFd, &epollEvent, 1, -1)) {
			error(1, errno, "Epoll failed");
			ctrl_c(SIGINT);
		}
		((Handler*)epollEvent.data.ptr)->handle(epollEvent.events);
	}
}

void ctrl_c(int) {
	close(serverSocket);
	printf("Closing server\n");
	exit(0);
}
#include "HangmanServer.h"

using namespace std;

int epollFd;
int serverSocket;
void ctrl_c(int);

class Room {

};

class Game {
public:
	static Game& Instance() {
		static Game instance;
		return instance;
	}

	int GetFreeRoomId() {
		for (int i = LOWER_BOUND; i <= UPPER_BOUND; i++) {
			if (!(roomMap.find(i) == roomMap.end())) {
				return i;
			}
		}
	}

private:
	const int LOWER_BOUND = 0;
	const int UPPER_BOUND = 5000;

	std::map<int, std::shared_ptr<Room>> roomMap;
	
	Game() {};
	Game(const Game&) = delete;
	Game(Game&&) = delete;
	void operator=(const Game&) = delete;
	void operator=(const Game&&) = delete;
};

enum OperationCodes {
	SendNewRoomId
};

class Handler {
public:
	virtual ~Handler() {};
	virtual void Handle(uint events) = 0;
};

class Player : public Handler {
public:
	Player(int socket) {
		_socket = socket;
		epoll_event ee{ EPOLLIN | EPOLLRDHUP, {.ptr = this} };
		epoll_ctl(epollFd, EPOLL_CTL_ADD, _socket, &ee);
	}

	~Player() {
		epoll_ctl(epollFd, EPOLL_CTL_DEL, _socket, nullptr);
		shutdown(_socket, SHUT_RDWR);
		close(_socket);
	}
	
	void Handle(uint events) override {
		if (events & EPOLLIN) {
            char buffer[BUFFER_SIZE];
			memset(buffer, 0x00, BUFFER_SIZE);
            ssize_t count = read(_socket, buffer, BUFFER_SIZE - 1);
			buffer[count] = '\0';
			
			if (count <= 0) {
				events |= EPOLLERR;
			}
			else {
				currentMessagesToRead.insert(currentMessagesToRead.end(), buffer, buffer + count);
				string tmpMessage;

				for (int i = 0; i < currentMessagesToRead.size(); i++) {
					if (currentMessagesToRead[i] == '\n') {
						tmpMessage += currentMessagesToRead[i];
						ParseMessage(tmpMessage);
						tmpMessage.clear();
					}
					else {
						tmpMessage += currentMessagesToRead[i];
					}
				}

				currentMessagesToRead.clear();

				if (tmpMessage.size() != 0) {
					currentMessagesToRead.assign(tmpMessage.begin(), tmpMessage.end());
				}
			}
		}

		if (events & EPOLLOUT) {
			bool dataToSend = currentMessagesToSend.size() > 0 ? true : false;
			while (dataToSend) {
				int i = currentMessagesToSend.size() - 1;
				const char* data = currentMessagesToSend[i].c_str();
				size_t size = currentMessagesToSend[i].size();
				int sent = send(_socket, data, size, MSG_DONTWAIT);

				if (sent == -1) {
					if (errno != EWOULDBLOCK && errno != EAGAIN) {
						events |= EPOLLERR;
					}

					break;
				}
				else if(sent != size) {
					currentMessagesToSend[i] = currentMessagesToSend[i].substr(sent);
				}
				else {
					currentMessagesToSend.pop_back();
				}

				if (currentMessagesToSend.size() == 0)
					dataToSend = false;
			}

			if (currentMessagesToSend.size() == 0)
				WaitForWrite(false);
		}

		if (events & ~(EPOLLIN | EPOLLOUT)) {
			delete this;
		}
	}

private:
	const size_t BUFFER_SIZE = 8;
	int _socket;
	std::vector<char> currentMessagesToRead;
	std::vector<string> currentMessagesToSend;

	void WaitForWrite(bool epollout) {
		epoll_event ee{ { EPOLLIN | (epollout ? EPOLLOUT : 0) | EPOLLRDHUP }, {.ptr = this} };
		epoll_ctl(epollFd, EPOLL_CTL_MOD, _socket, &ee);
	}

	void PrepereToSend(std::vector<string>& messageParts) {
		string fullMessage;

		for (int i = 0; i < messageParts.size(); i++) {
			fullMessage += messageParts[i];
			fullMessage += " ";
		}

		fullMessage += "\n";

		WaitForWrite(true);
		currentMessagesToSend.emplace_back(fullMessage);
	}

	void HandleOperation(std::vector<string>& splitted) {
		OperationCodes operationType = (OperationCodes)stoi(splitted[0]);
		std::vector<string> toSend;

		switch (operationType)
		{
		case OperationCodes::SendNewRoomId:
			toSend.emplace_back("message sent!");
		default:
			break;
		}

		if(toSend.size() != 0) 
			PrepereToSend(toSend);
	}

	void ParseMessage(string message) {
		std::vector<string> splitted;
		string messagePart;

		for (int i = 0; i < message.size(); i++) {
			if (message[i] == '\n' && messagePart.size() != 0) {
				splitted.emplace_back(messagePart);
			}
			else if (message[i] == ' ') {
				splitted.emplace_back(messagePart);
				messagePart.clear();
			}
			else {
				messagePart += message[i];
			}
		}

		HandleOperation(splitted);
	}
};

class ServerHandler : public Handler {
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

	void Handle(uint events) override {
		if (events & EPOLLIN) {
			sockaddr_in clientAddr{};
			socklen_t clientAddrSize;
			clientAddrSize = sizeof(clientAddr);
			int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);

			Player* player = new Player(clientSocket);
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

		((Handler*)epollEvent.data.ptr)->Handle(epollEvent.events);
	}
}

void ctrl_c(int) {
	close(serverSocket);
	printf("Closing server\n");
	exit(0);
}
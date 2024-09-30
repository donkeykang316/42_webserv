#include "Server.hpp"

Server::Server() {
	struct addrinfo hints;
	struct addrinfo *res;
	struct addrinfo *rp;

	memset(&hints, 0 , sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_flags = AI_PASSIVE;

	const char *port = "5000";
	int status = getaddrinfo(NULL, port, &hints, &res);
	if (status != 0) {
		std::cerr << gai_strerror(status);
	}

	for (rp = res; rp != NULL; rp = rp->ai_next) {
		_socketFD = socket(AF_INET, SOCK_STREAM, 0);
		if (_socketFD < 0) {
			std::cerr << "socket ERROR\n";
		}

		int opt = 1;
		if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
			std::cerr << "setsocket ERROR\n";
			close(_socketFD);
			break;
		}
	
		if (bind(_socketFD, rp->ai_addr, rp->ai_addrlen) == 0) {
			std::cout << "binding success\n";
			break;
		}
		close(_socketFD);
	}

	if (rp == NULL) {
		std::cerr << "bind failed\n";
		close(_socketFD);
	}

	if (listen(_socketFD, 5) < 0) {
		std::cerr << "listen failed\n";
		close(_socketFD);
	}

	serverInit();

	std::cout << "Server is listening on port 5000\n";
	while (1)
	{
		socklen_t clientAddrLen = sizeof(_clientAddr);
		_clientSocketFD = accept(_socketFD, (struct sockaddr *)&_clientAddr, &clientAddrLen);
		if (_clientSocketFD < 0) {
			std::cerr << "Could not connect with the client!\n";
		}
		// Prepare an HTTP response
		std::string httpResponse = "HTTP/1.1 200 OK\n"
			"\n"
			+ _htmlFile["server/index.html"];

    	if (send(_clientSocketFD, httpResponse.c_str(), httpResponse.size(), 0) < 0) {
        	std::cerr << "Failed to send response to the client.\n";
    	}
		close(_clientSocketFD);
	}
	freeaddrinfo(res);
}

Server::~Server() {
	close(_socketFD);
}

void Server::serverInit() {
	std::string key = "server/index.html";
	std::ifstream file;
	file.open(key.c_str());
	if (!file.is_open()) {
        throw std::exception();
    }
	std::string line;
    while (std::getline(file, line)) {
		_htmlFile[key] += line + "\n";
	}
}
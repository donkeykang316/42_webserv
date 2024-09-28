#include "Server.hpp"

Server::Server() {
	/*
	socket funtion breakdown:
		Domain value: 
			AF_INET: IPv4 Internet protocols
			AF_INET6: IPv6 Internet protocols
			AF_UNIX or AF_LOCAL: Local communication (on the same machine)
		Type value:
			SOCK_STREAM: Provides sequenced, reliable, two-way, connection-based byte streams. Corresponds to the TCP protocol
			SOCK_RAW: Provides raw network protocol access
	*/
	_socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFD < 0) {
		std::cerr << "socket ERROR\n";
	}

	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_port = htons(5000);
	/*
	struct in_addr {
    	uint32_t s_addr; // 32-bit IPv4 address (in network byte order)
	};
	To listen specific IP use ...
	*/
	_serverAddr.sin_addr.s_addr = INADDR_ANY;
	if (bind(_socketFD, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) < 0) {
		std::cerr << "bind failed\n";
		close(_socketFD);
	}

	if (listen(_socketFD, 5) < 0) {
		std::cerr << "listen failed\n";
		close(_socketFD);
	}

	std::cout << "Server is listening on port 5000\n";
	socklen_t clientAddrLen = sizeof(_clientAddr);
	_clientSocketFD = accept(_socketFD, (struct sockaddr *)&_clientAddr, &clientAddrLen);
	if (_clientSocketFD < 0) {
		std::cerr << "Could not connect with the client!\n";
	}
	// Prepare an HTTP response
    std::string body = "<html><body><h1>Welcome to the Awesome Server!</h1></body></html>";
    std::stringstream ss;
    ss << body.size();
    std::string contentLength = ss.str();
	std::string httpResponse =
        	"HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=UTF-8\r\n"
            "Content-Length: " + contentLength + "\r\n"
            "Connection: close\r\n"
            "\r\n" +
            body;

    // Send the HTTP response to the client
    if (send(_clientSocketFD, httpResponse.c_str(), httpResponse.size(), 0) < 0) {
        std::cerr << "Failed to send response to the client.\n";
    }
	close(_clientSocketFD);
}

Server::~Server() {
	close(_socketFD);
}
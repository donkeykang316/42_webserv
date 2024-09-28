#include "Server.hpp"

Server::Server() {
	_socketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFD < 0) {
		std::cerr << "socket ERROR\n";
	}

	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_port = htons(5000);
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
	while (1)
	{
		socklen_t clientAddrLen = sizeof(_clientAddr);
		_clientSocketFD = accept(_socketFD, (struct sockaddr *)&_clientAddr, &clientAddrLen);
		if (_clientSocketFD < 0) {
			std::cerr << "Could not connect with the client!\n";
		}
		// Prepare an HTTP response
   		std::string body = "Welcome to the Awesome Server!";
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

    	if (send(_clientSocketFD, httpResponse.c_str(), httpResponse.size(), 0) < 0) {
        	std::cerr << "Failed to send response to the client.\n";
    	}
		close(_clientSocketFD);
	}
}

Server::~Server() {
	close(_socketFD);
}
#include "Client.hpp"

Client::Client() {
	// Create a socket
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == -1) {
		std::cerr << "Could not create a socket!" << std::endl;
	}
	std::cout << "Socket created!" << std::endl;

	// Set the server address
	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_port = htons(5000);
	_serverAddress.sin_addr.s_addr = INADDR_ANY;

	// Connect to the server
	int connectionStatus = connect(_socket, (struct sockaddr *)&_serverAddress, sizeof(_serverAddress));
	if (connectionStatus == -1) {
		std::cerr << "Could not connect to the server!" << std::endl;
	}

	// receive a message from the server
	recv(_socket, _message, sizeof(_message), 0);
	std::cout << "Server: " << std::string(_message, 0, sizeof(_message)) << std::endl;

	close(_socket);
}

Client::~Client () {}
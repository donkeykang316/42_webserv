#include "Server.hpp"

Server::Server() {
	// Create a socket
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == -1) {
		std::cerr << "Could not create a socket!" << std::endl;
	}

	// Bind the socket to an IP / port
	_serverAddress.sin_family = AF_INET;
	_serverAddress.sin_port = htons(5000);
	_serverAddress.sin_addr.s_addr = INADDR_ANY;
	bind(_socket, (struct sockaddr *)&_serverAddress, sizeof(_serverAddress));

	// Tell the socket is going to be used for listening
	listen(_socket, 5);

	// Wait for a connection
	int clientSocket = accept(_socket, (struct sockaddr *)&_clientAddress, (socklen_t*)&_clientAddress);
	if (clientSocket == -1) {
		std::cerr << "Could not connect with the client!" << std::endl;
	}

	// Send a message to the client
	std::string welcomeMessage = "Welcome to the Awesome Server!";
	send(clientSocket, welcomeMessage.c_str(), welcomeMessage.size() + 1, 0);

	close(clientSocket);
}

Server::~Server() {}
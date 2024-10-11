#include "cgi.hpp"

int main() {
	int	_clientSocketFD;
	int	socketFD;
	struct sockaddr_in	_clientAddr;
	std::string portInfo = "8000";
	
	socketFD = createListenSocket(portInfo);

	std::cout << "Server is listening on port 5000\n";
	socklen_t clientAddrLen = sizeof(_clientAddr);
	_clientSocketFD = accept(socketFD, (struct sockaddr *)&_clientAddr, &clientAddrLen);
	if (_clientSocketFD < 0) {
		std::cerr << "Could not connect with the client!\n";
	}
	// Prepare an HTTP response
	//executeCGI();
	std::string body = "Welcome to the Awesome Server!";
	std::string httpResponse = "HTTP/1.1\n\n" + body;

	// Send the HTTP response to the client
	if (send(_clientSocketFD, httpResponse.c_str(), httpResponse.size(), 0) < 0) {
    	std::cerr << "Failed to send response to the client.\n";
	}
	close(_clientSocketFD);
	close(socketFD);
}
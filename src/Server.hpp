#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

class Server {
private:
	int 				_socket;
	struct sockaddr_in	_serverAddress;
	struct sockaddr_in	_clientAddress;
	//char 				_message[4096];

public:
	Server();
	~Server();
};
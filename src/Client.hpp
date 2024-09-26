#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

class Client {
private:
	int 				_socket;
	struct sockaddr_in	_serverAddress;
	char 				_message[4096];

public:
	Client();
	~Client();

};
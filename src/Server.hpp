#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <sys/socket.h>
#include <netdb.h>

class Server {
private:
	int					_socketFD;
	int					_clientSocketFD;
	struct sockaddr_in	_serverAddr;
	struct sockaddr_in	_clientAddr;
	
public:
	Server();
	~Server();
};
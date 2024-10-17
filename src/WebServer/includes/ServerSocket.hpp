#ifndef SERVERSOCKET_H
# define SERVERSOCKET_H

# include <string>
# include <cstring>
# include <fcntl.h>
# include <unistd.h>
# include <iostream>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <sys/poll.h>
# include <errno.h>

# include "WebServer.hpp"

class ServerSocket
{
private:
	int socketFd;
	std::vector<WebServer*> servers;
	std::map<std::string, WebServer*> hostToServer;
	int createServerSocket(std::string port);
public:
	ServerSocket(std::string &port, WebServer *server);
	~ServerSocket();
	void updateServerSocket(WebServer *server);
	int getFd();
	WebServer *getServer(std::string host);
};

#endif

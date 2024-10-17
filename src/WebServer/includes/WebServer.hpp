#ifndef WEBSERVER_H
# define WEBSERVER_H

# include <fcntl.h>
# include <unistd.h>
# include <iostream>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
#include <sys/poll.h>

# include "../../Configuration/includes/Dictionary.hpp"
# include "../../Configuration/includes/ServerConfig.hpp"
# include "../../HTTP/includes/HTTPRequest.hpp"
# include "../../HTTP/includes/HTTPResponse.hpp"

#define BACKLOG 10     // how many pending connections queue will hold

class WebServer
{
private:
	// std::map<std::string, int> socket_fds;
	// std::map<std::string, int>	newSocketFds;
	// struct pollfd fds[200];
	// char	_buffer[30000];
	struct addrinfo *res;

	ServerConfig *serverConfig;
	Dictionary &dictionary;
	// HTTPRequest *curr_request;

	// void	_accept();
	HTTPRequest	*_handle(char *buffer);
	void	_respond(int newSocketFd, HTTPRequest *request);
public:
	WebServer(ServerConfig *serverConfigElem, Dictionary &dictionary);
	WebServer& operator=(const WebServer &rhs);
	~WebServer();
	// void launch();

	std::set<std::string> getServerNameAliases();
	std::string getResponseErrorFilePath(LocationConfig *location, enum status_code_value statusCode);
	std::string getResponseFilePath(HTTPRequest *request);
};

#endif

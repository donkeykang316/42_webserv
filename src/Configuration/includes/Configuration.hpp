#ifndef CONFIGURATION_H
# define CONFIGURATION_H

# include <fstream>
# include <string>
# include <vector>
# include <stack>
# include <algorithm>

# include "../../HTTP/includes/HTTP.h"
# include "Dictionary.hpp"
# include "AAttribute.hpp"
# include "LocationConfig.hpp"
# include "ServerConfig.hpp"
# include "../../WebServer/includes/ServerSocket.hpp"
# include "../../HTTP/includes/CGI.hpp"

class Configuration
{
private:
	const char *configFile;
	std::vector<WebServer*>_webServers;
	std::map<std::string, ServerSocket*> _serverSockets;
	void _initServerSockets();
	bool setNonBlocking(int sockfd);
public:
	Dictionary dictionary;
	int blockLayer;
	int lineNb;
	std::vector<ServerConfig*> servers;
	Configuration(const char *path);
	~Configuration();
	void parseConfig();
	void printConfigurationData();
	void start();

	void handleNewConnections(fd_set& read_fds, std::vector<int>& clientSockets, std::map<int, HTTPRequest*>& httpRequests);
	void handleClientCommunications(fd_set& read_fds, std::vector<int>& clientSockets, std::map<int, HTTPRequest*>& httpRequests);
	bool processClientRequest(int clientfd, HTTPRequest* request);
	void closeClientConnection(int clientfd, std::vector<int>& clientSockets, std::map<int, HTTPRequest*>& httpRequests, size_t index);
};

#endif

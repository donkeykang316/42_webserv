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
# include "../../WebServer/includes/CGI.hpp"

class Configuration
{
private:
	const char *configFile;
	std::vector<WebServer*>_webServers;
	std::map<std::string, ServerSocket*> _serverSockets;
	void _initServerSockets();
	bool setNonBlocking(int sockfd);
	cgi	_cgi;
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
};

#endif

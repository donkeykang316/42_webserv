#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP
/*
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <fstream>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <cstdlib>
#include <csignal>
#include <sys/poll.h>
*/
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

//#define BACKLOG 10 

class Conf {
private:
	typedef struct ErrorPage {
		std::vector<int>	errorCodes;
		std::string			path;
	}	errorPage;

	typedef struct LocationConfig {
		std::string							path;
		std::string							root;
		std::string							modifier;
		bool								autoindex;
		int									clientMAxSize;
		std::vector<std::string>			limitExceptMethods;
		std::vector<errorPage>				errorPages;
		std::map<std::string, std::string>	cgiParams;
        std::string 						cgiPass;
        std::string 						cgiIndex;
        std::string 						returnDirective;

	}	locationConfig;

	typedef struct ServerConfig {
		int							listen;
		bool						isDefaultServer;
		std::vector<std::string>	serverNames;
		std::string					root;
		std::vector<std::string>	index;
		bool						autoIndex;
		int							clientMAxSize;
		std::vector<errorPage>		errorPages;
		std::vector<locationConfig>	locations;

	}	serverConfig;

	typedef struct HttpConfig {
		std::vector<std::string>	includes;
		std::string					defaultType;
		std::vector<errorPage>		errorPages;
		int							clientMAxSize;
		std::vector<serverConfig>	servers;

	}	httpConfig;

	typedef struct EventsConfig {
		int		worker_connections;

	}	eventsConfig;

	typedef struct Config {
		eventsConfig	events;
		httpConfig		http;

	} config;

public:
	Conf();
	~Conf();
};

#endif
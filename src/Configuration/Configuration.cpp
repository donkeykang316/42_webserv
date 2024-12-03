#include "includes/Configuration.hpp"

// Configuration
Configuration::Configuration(const char *path) : configFile(path)
{
	blockLayer = 0;
	lineNb = 0;
	servers.clear();

	parseConfig();
	_initServerSockets();
}

Configuration::~Configuration()
{
	for (std::map<std::string, ServerSocket*>::iterator serverSocketIt = _serverSockets.begin(); serverSocketIt != _serverSockets.end(); serverSocketIt++)
		delete serverSocketIt->second;
	for (std::vector<WebServer*>::iterator webServer = _webServers.begin(); webServer != _webServers.end(); webServer++)
		delete *webServer;
	for (std::vector<ServerConfig*>::iterator server = servers.begin(); server != servers.end(); server++)
		delete *server;
}

void Configuration::parseConfig()
{
	std::stack<std::string> blockInProgressStack;
	std::string blockStart = "{";
	std::string blockEnd = "}";

	std::ifstream nameFileOut;
	nameFileOut.open(configFile);

	std::string configLine;
	std::vector<std::string> confLineVector;

	ServerConfig *server = new ServerConfig(dictionary);
	LocationConfig location(dictionary, server->errorPages, server->clientMaxBodySize);

	while (std::getline(nameFileOut, configLine))
	{
		lineNb++;

		configLine.erase(0, configLine.find_first_not_of(" \r\t\n"));
		if (configLine.size() == 0 || configLine[0] == '#')
			continue;
		while (configLine.size())
		{
			std::string configLineElem = configLine.substr(0, configLine.find_first_of(" \r\t\n"));
			configLine.erase(0, configLineElem.size());
			if (configLineElem[0] == '#')
			{
				break;
			}
			confLineVector.push_back(configLineElem);
			configLine.erase(0, configLine.find_first_not_of(" \r\t\n"));
			bool isBlockStart = !configLineElem.compare(blockStart);
			bool isBlockEnds = !configLineElem.compare(blockEnd);
			int isNotAttributeEnd = 0 + configLineElem.at(configLineElem.size() - 1) - ';';

			// check error in structure cases:
			//	1. closing } without opening one
			//	2. incorrect level of block
			//	3. attributes are out of scope
			if ((isBlockEnds && blockLayer == 0) || (isBlockStart && blockLayer != dictionary.getConfigBlockLevel(confLineVector[0])) || (!blockLayer && !isNotAttributeEnd))
			{
				std::cout << "Incorrect configuration struct on line " << lineNb << std::endl;
				exit(1);
			}
			if (isBlockEnds)
			{
				switch (dictionary.getConfigBlockLevel(blockInProgressStack.top()))
				{
				case 1:
					if (location.isValid())
						server->addLocation(location);
					else
					{
						std::cout << "ERROR WITH VALIDATION OF LOCATION on line " << lineNb << std::endl;
						// exit(1);
					}
					break;
				case 0:
					if (server->isValid())
						this->servers.push_back(server);
					else
					{
						std::cout << "ERROR WITH VALIDATION OF SERVER on line " << lineNb << std::endl;
						delete server;
					// 	exit(1);
					}
					server = new ServerConfig(dictionary);
				default:
					break;
				}
				blockLayer--;
				blockInProgressStack.pop();
			}
			if (isBlockStart)
			{
				if (blockLayer == 0 && confLineVector.size() > 2)
				{
					std::cout << "Invalid instruction for " << confLineVector[0] << " on line " << lineNb << ". Extra parameters" << std::endl;
					exit(1);
				}
				else if (blockLayer == 1)
				{
					location = LocationConfig(dictionary, server->errorPages, server->clientMaxBodySize);
					location.setUri(confLineVector);
				}
				blockLayer++;
				blockInProgressStack.push(confLineVector[0]);
			}

			if (!isNotAttributeEnd)
			{
				int lastElemIdx = confLineVector.size() - 1;
				if (confLineVector[lastElemIdx].size() == 1)
					confLineVector.pop_back();
				else
					confLineVector[lastElemIdx].erase(confLineVector[lastElemIdx].find_last_of(';'));
				switch (dictionary.getConfigBlockLevel(blockInProgressStack.top()))
				{
				case 0:
					server->fillAttributes(confLineVector);
					break;
				case 1:
					location.fillAttributes(confLineVector, dictionary);
				default:
					break;
				}
			}
			if (isBlockStart || isBlockEnds || !isNotAttributeEnd)
				confLineVector.clear();
		}
	}
	delete server;
}

void Configuration::printConfigurationData()
{
	int serverIdx = 0;
	int locationsIdx = 0;

	std::vector<ServerConfig*>::iterator currServerIt = servers.begin();
	while (currServerIt != servers.end())
	{
		ServerConfig *currServer = * currServerIt;
		std::cout << "SERVER number " << serverIdx << std::endl;
		std::cout << "--- listen: ";
		std::set<std::string> listenPorts = currServer->getListenPorts();
		for (std::set<std::string>::iterator port = listenPorts.begin(); port != listenPorts.end(); port++)
			std::cout << *port << " ";
		std::cout << std::endl;
		std::cout << "--- index: " << currServer->index << std::endl;

		std::cout << "--- serverName: ";
		std::set<std::string> serverNameAliases = currServer->getServerNameAliases();
		for (std::set<std::string>::iterator serverNameAlias = serverNameAliases.begin(); serverNameAlias != serverNameAliases.end(); serverNameAlias++)
		std::cout << *serverNameAlias << " ";
		std::cout << std::endl;
		std::pair<std::string, std::string> redir = currServer->getRedirection();
		if (redir.first.size())
		{
			std::cout << "--- redirection port: " << redir.first << " url " << redir.second << std::endl;
		}
		std::cout << "--- clientMaxBodySize: " << currServer->clientMaxBodySize << std::endl;
		std::cout << "--- ERROR PAGES: " << std::endl;
		std::map<std::string, std::vector<std::string> >::iterator itErr;

		for (itErr = currServer->errorPages.begin(); itErr != currServer->errorPages.end(); itErr++)
		{
			std::cout << "------ " << itErr->first // string (key)
					  << ':';
			std::vector<std::string>::iterator itErrPage;
			for (itErrPage = itErr->second.begin(); itErrPage != itErr->second.end(); itErrPage++)
				std::cout << *itErrPage << " "; // string's value
			std::cout << std::endl;
		}
		serverIdx++;
		std::vector<LocationConfig>::iterator currLocation = currServer->locations.begin();
		while (currLocation != currServer->locations.end())
		{
			std::cout << std::endl
					  << "--- LOCATION number " << locationsIdx << std::endl;
			std::cout << "------ is CGI: " << currLocation->isCgi << std::endl;
			std::cout << "------ uri: " << currLocation->uri << std::endl;
			std::cout << "------ modifier: " << currLocation->modifier << std::endl;
			std::cout << "------ index: " << currLocation->index << std::endl;
			std::cout << "------ cgiPass: " << currLocation->cgiPass << std::endl;
			std::cout << "------ cgiIndex: " << currLocation->cgiIndex << std::endl;
			std::cout << "------ cgiInclude: " << currLocation->cgiInclude << std::endl;
			std::cout << "------ CGIParams: " << std::endl;
			for (std::map<std::string, std::string>::iterator it = currLocation->cgiParams.begin(); it != currLocation->cgiParams.end(); it++)
			{
				std::cout << "--------- " << it->first // string (key)
						  << ':' << it->second << std::endl;
			}
			std::cout << "------ ALLOWED METHODS: " << std::endl;
			std::set<std::string>::iterator itMethod;
			std::cout << "[ ";
			for (itMethod = currLocation->allowedMethods.begin(); itMethod != currLocation->allowedMethods.end(); itMethod++)
			{
				std::cout << itMethod->data() << " ";
			}
			std::cout << "]" << std::endl;
			std::pair<std::string, std::string> redirLoc = currLocation->getRedirection();
			if (redirLoc.first.size())
			{
				std::cout << "--- redirection port: " << redirLoc.first << " url " << redirLoc.second << std::endl;
			}
			std::cout << "------ ERROR PAGES: " << std::endl;

			for (itErr = currLocation->errorPages.begin(); itErr != currLocation->errorPages.end(); itErr++)
			{
				std::cout << "--------- " << itErr->first // string (key)
						  << ':';
				std::vector<std::string>::iterator itErrPage;
				for (itErrPage = itErr->second.begin(); itErrPage != itErr->second.end(); itErrPage++)
					std::cout << *itErrPage << " "; // string's value
				std::cout << std::endl;
			}

			currLocation++;
			locationsIdx++;
		}
		currServerIt++;
		locationsIdx = 0;
	}
};

void Configuration::_initServerSockets()
{
	// create server for  each configuration item
	for (std::vector<ServerConfig*>::iterator serverConfigIt = servers.begin(); serverConfigIt != servers.end(); serverConfigIt++)
	{
		ServerConfig *serverConfig = *(serverConfigIt);
		WebServer *server = new WebServer(serverConfig, dictionary);
		std::cout << "TEST INNER SERVER_CONF DATA" << std::endl;
		std::cout << *server->getServerNameAliases().begin() << std::endl;
		_webServers.push_back(server);
		// take all ports for server
		std::set<std::string> listenPorts = serverConfig->getListenPorts();
		for(std::set<std::string>::iterator portInfoIt = listenPorts.begin(); portInfoIt != listenPorts.end(); portInfoIt++)
		{
			// check on existence of listening socket for current port
			std::string portInfo = *portInfoIt;
			if (_serverSockets.find(portInfo) == _serverSockets.end())
			{
				ServerSocket *socket = new ServerSocket(portInfo, server);
				_serverSockets[portInfo] = socket;
			std::cout << "PORT TO SOCKET " << portInfo << " to " << socket->getFd() << std::endl;
			}
			else
			{
				_serverSockets[portInfo]->updateServerSocket(server);
			std::cout << "PORT TO SOCKET " << portInfo << " to " << "NONE" << std::endl;
			}
		}
	}
}

bool Configuration::setNonBlocking(int sockfd)
{
	int flags = fcntl(sockfd, F_GETFL, 0);
	if (flags == -1)
		return false;
	flags |= O_NONBLOCK;
	return (fcntl(sockfd, F_SETFL, flags) != -1);
}

void Configuration::start()
{
	// Set up the master file descriptor set
	fd_set			master_set, server_set;
	fd_set			read_fds;
	struct timeval	timeout;

	FD_ZERO(&master_set);
	FD_ZERO(&read_fds);
	FD_ZERO(&server_set);
	int fd_max = 0;

	// Add listening sockets to the master set
	for (std::map<std::string, ServerSocket *>::iterator _listenSocketIt = _serverSockets.begin(); _listenSocketIt != _serverSockets.end(); ++_listenSocketIt)
	{
		std::cout <<"listenSocketIt->second->getFd() " << (_listenSocketIt->first) << std::endl;
		std::cout <<"listenSocketIt->second->getFd() " << (_listenSocketIt->second->getFd()) << std::endl;
		ServerSocket *socket = _listenSocketIt->second;
		int serverSocketFd = socket->getFd();
		FD_SET(serverSocketFd, &master_set);
		FD_SET(serverSocketFd, &server_set);
		if (serverSocketFd > fd_max)
		{
			fd_max = serverSocketFd;
		}
	}

	// Initialize the timeval struct to 3 minutes.  If no
   	// activity after 3 minutes this program will end.
	timeout.tv_sec  = 3 * 60;
	timeout.tv_usec = 0;
	// Vector to keep track of client sockets
	// std::vector<int> clientSockets;
	std::map<int, HTTPRequest*> httpRequests;
	// clientSockets.clear();
	httpRequests.clear();

	while (true)
	{
		memcpy(&read_fds, &master_set, sizeof(master_set));
		std::cout << "Waiting on select()..." <<std::endl;
		int activity = select(fd_max + 1, &read_fds, NULL, NULL, &timeout);
		if (activity < 0)
		{
			if (errno == EINTR)
				continue; // Interrupted by signal
			std::cerr << "select" << errno << std::endl;
			break;
		}
		// Check to see if the 3 minute time out expired.
		if (activity == 0)
		{
			std::cerr << "  select() timed out.  End program." << std::endl;
			break;
		}

		int descReady = activity;
		for (int i = 0; i <= fd_max  &&  descReady > 0; ++i)
		{
			 if (FD_ISSET(i, &read_fds))
			{
				descReady -= 1;
				if (FD_ISSET(i, &server_set))
				{
					std::cout << "  Listening socket is readable\n" << std::endl;
					while (true)
					{
						int newSd = accept(i, NULL, NULL);
						if (newSd < 0)
						{
							if (errno != EWOULDBLOCK)
							{
								std::cerr << "  accept() failed" << std::endl;
							}
							break;
						}
						std::cout <<"  New incoming connection - " << newSd << std::endl;
						FD_SET(newSd, &master_set);
						if (newSd > fd_max)
							fd_max = newSd;
						HTTPRequest *clientRequest = new HTTPRequest(newSd, (this->dictionary));
						httpRequests[newSd] = clientRequest;
					}
				}
				else
				{
					std::cout << "  Descriptor " << i << " is readable" << std::endl;
					unsigned char buffer[4096];
					ssize_t rc = 0;
					HTTPRequest *clientRequest = httpRequests[i];
					while (true)
					{
						/**********************************************/
						/* Receive data on this connection until the  */
						/* recv fails with EWOULDBLOCK.  If any other */
						/* failure occurs, we will close the          */
						/* connection.                                */
						/**********************************************/
						memset(buffer, 0, 4096);
						rc = recv(i, buffer, sizeof(buffer) - 1, 0);


						if ( rc > 0 )
							buffer[rc] = '\0';
						if (rc < 0)
						{
							clientRequest->isFulfilled = true;
							if (errno != EWOULDBLOCK)
							{
							perror("  recv() failed");
							// close_conn = TRUE;
							}
							break;
						}
						if (rc == 0)
						{
							clientRequest->isFulfilled = true;
							delete httpRequests[i];
							clientRequest = NULL;
							printf("  Connection closed\n");
							close(i);
							FD_CLR(i, &master_set);
							if (i == fd_max)
							{
								while (FD_ISSET(fd_max, &master_set) == false)
									fd_max -= 1;
							}
							break;
						}
						clientRequest->fillRequestData(buffer, rc);
						if (clientRequest->response == NULL)
						{
							std::cout << "RESPONSE CREATE" << std::endl;
							std::string hostAndPort = clientRequest->headers["Host"];
							std::string port = hostAndPort.substr(hostAndPort.find_first_of(':') + 1);
							std::string host = hostAndPort.substr(0, hostAndPort.find_first_of(':'));
							WebServer *currServer = _serverSockets[port]->getServer(port);
							std::string responseFile = currServer->getResponseFilePath(clientRequest);
							HTTPResponse *response = new HTTPResponse(i, *clientRequest, responseFile);
							clientRequest->response = response;
							if (clientRequest->isHeadersSet && clientRequest->getBuffer().size())
							{
								clientRequest->fillRequestData((const unsigned char *)"", 0);
							}
						}
						std::cout << BOLDYELLOW << "clientRequest->response->isFulfilled " << clientRequest->response->isFulfilled << RESET << std::endl;
						if (clientRequest && clientRequest->response && clientRequest->isFulfilled)
						{
							std::cout << YELLOW << "SEND RESPOSE FULFILLED" << RESET << std::endl;

							clientRequest->response->sendResponse();
							std::cout << BOLDYELLOW << "SEND configuration" << RESET << std::endl;
							send(i , clientRequest->response->response.c_str(), clientRequest->response->response.size(),0);
							delete httpRequests[i];
							httpRequests[i] = new HTTPRequest(i, (this->dictionary));
							clientRequest = httpRequests[i];
						}
					};
				}
			}
		}
	}

	// Clean up all of the sockets that are open
	for (int i=0; i <= fd_max; ++i)
	{
		if (FD_ISSET(i, &master_set))
			close(i);
	}
}

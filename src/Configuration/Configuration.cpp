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
		std::cout << "TEST INNER SERVERCONF DATA" << std::endl;
		std::cout << *server->getServerNameAliases().begin() << std::endl;
		_webServers.push_back(server);
		// take all ports for server
		std::set<std::string> listenPorts = serverConfig->getListenPorts();
		for(std::set<std::string>::iterator portInfoIt = listenPorts.begin(); portInfoIt != listenPorts.end(); portInfoIt++)
		{
			// check on existance of listening socket for current port
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
	int a = 3;
	std::cout <<"Start " << a << std::endl;
	// Set up the master file descriptor set
	fd_set master_set;
	fd_set read_fds;
	FD_ZERO(&master_set);
	FD_ZERO(&read_fds);
	int fd_max = 0;

	// Add listening sockets to the master set
	for (std::map<std::string, ServerSocket *>::iterator _listenSocketIt = _serverSockets.begin(); _listenSocketIt != _serverSockets.end(); ++_listenSocketIt)
	{
		std::cout <<"listenSocketIt->second->getFd() " << (_listenSocketIt->first) << std::endl;
		std::cout <<"listenSocketIt->second->getFd() " << (_listenSocketIt->second->getFd()) << std::endl;
		ServerSocket *socket = _listenSocketIt->second;
		int serverSocketFd = socket->getFd();
		FD_SET(serverSocketFd, &master_set);
		if (serverSocketFd > fd_max)
		{
			fd_max = serverSocketFd;
		}
	}

	// Vector to keep track of client sockets
	std::vector<int> clientSockets;
	std::map<int, HTTPRequest*> httpRequests;
	clientSockets.clear();
	httpRequests.clear();
	while (true)
	{
		read_fds = master_set;

		int activity = select(fd_max + 1, &read_fds, NULL, NULL, NULL);
		if (activity < 0)
		{
			if (errno == EINTR)
				continue; // Interrupted by signal
			std::cerr << "select" << errno << std::endl;
			break;
		}

		// Iterate through listening sockets to check for new connections
		for (std::map<std::string, ServerSocket *>::iterator _listenSocketIt = _serverSockets.begin(); _listenSocketIt != _serverSockets.end(); ++_listenSocketIt)
		{
			int _listenSocket = _listenSocketIt->second->getFd();
			int sockfd = _listenSocket;
			if (FD_ISSET(sockfd, &read_fds))
			{
				struct sockaddr_in client_addr;
				socklen_t addrlen = sizeof(client_addr);
				int newfd = accept(sockfd, (struct sockaddr *)&client_addr, &addrlen);
				if (newfd < 0)
				{
					if (errno != EWOULDBLOCK && errno != EAGAIN)
					{
						std::cerr << "accept" << errno << std::endl;
					}
					continue;
				}
				if (!setNonBlocking(newfd))
				{
					std::cerr << "fcntl" << errno << std::endl;
					close(newfd);
					continue;
				}

				// Add the new socket to the master set and client list
				FD_SET(newfd, &master_set);
				if (newfd > fd_max)
					fd_max = newfd;
				clientSockets.push_back(newfd);
				HTTPRequest *clientRequest = new HTTPRequest((this->dictionary));
				httpRequests[newfd] = clientRequest;
			}
		}

		// Iterate through client sockets to check for incoming data
		for (size_t i = 0; i < clientSockets.size(); ++i)
		{
			int clientfd = clientSockets[i];
			HTTPRequest clientRequest = *httpRequests[clientfd];
			if (FD_ISSET(clientfd, &read_fds))
			{
				// Read client's request
				char buffer[1024] = "";
				ssize_t bytesReceived = recv(clientfd, buffer, sizeof(buffer) - 1, 0);
				if (bytesReceived == -1)
				{
					std::cerr << "Failed to read request from client: " << strerror(errno) << "\n";
					httpRequests.erase(clientfd);
					close(clientfd);
					FD_CLR(clientfd, &master_set);
					// Remove from clientSockets
					clientSockets.erase(clientSockets.begin() + i);
					--i; // Adjust index after removal
					continue;
				}
				while (bytesReceived > 0)
				{
					std::cout << "BUFFER: " << buffer << "|BUFFER eND" << std::endl;
					clientRequest.fillRequestData(buffer);
					if (clientRequest.response == NULL)
					{
						std::string hostAndPort = clientRequest.headers["Host"];
						std::string port = hostAndPort.substr(hostAndPort.find_first_of(':') + 1);
						std::string host = hostAndPort.substr(0, hostAndPort.find_first_of(':'));
						WebServer *currServer = _serverSockets[port]->getServer(port);
						std::string responseFile = currServer->getResponseFilePath(&clientRequest);
						HTTPResponse *response = new HTTPResponse(clientfd, clientRequest, responseFile);
						clientRequest.response = response;

					}
					bytesReceived = recv(clientfd, buffer, sizeof(buffer) - 1, 0);
				}
				clientRequest.isFulfilled = true;
				clientRequest.fillRequestData("");
				// std::cout << "CLIENT RESPONSE " << clientRequest.response->response << "|" << std::endl;
				// if (!clientRequest.location->isCgi)
					send(clientfd , clientRequest.response->response.c_str(), clientRequest.response->response.size(),0);

				// std::cout << "-----------NEW REQUEST----------" <<std::endl;
				// std::cout << buffer << std::endl;
				// std::cout << "-----------END OF REQUEST----------" <<std::endl;
				// // Remove
				// HTTPRequest *request = new HTTPRequest(buffer, dictionary);
				// std::string hostAndPort = request->headers["Host"];
				// // std::cout << " HOST " << hostAndPort << std::endl;
				// std::string port = hostAndPort.substr(hostAndPort.find_first_of(':') + 1);
				// std::string host = hostAndPort.substr(0, hostAndPort.find_first_of(':'));
				// // std::cout << " port " << port << " host " << host << std::endl;

				// // Move to Obj

				// WebServer *currServer = _serverSockets[port]->getServer(port);
				// // std::cout << "SERVER " << currServer->getServerNameAliases().begin()._M_node << std::endl;
				// // std::cout << " currServer " << *currServer->getServerNameAliases().begin() << std::endl;


				// std::string responseFile = currServer->getResponseFilePath(request);


				// std::cout << "********responseFile " << responseFile << std::endl;
				// if (request->location->isCgi)
				// {
				// 	currServer->runCGI(request->location, request);
				// }
				// else
				// {
				// 	HTTPResponse response(*request, responseFile);
				// 	delete request;
				// 	std::cout << "RESPONSE DATA: " << response.response << std::endl;
				// 	send(clientfd ,response.response.c_str(), response.response.size(),0);
				// }

				// Move to Obj
				// while (!clientRequest.isFulfilled)
					// sleep(1);
				if (clientRequest.isFulfilled)
				{
					// sleep(5);
					delete httpRequests[clientfd];
					httpRequests.erase(clientfd);
					close(clientfd);
					FD_CLR(clientfd, &master_set);
					clientSockets.erase(clientSockets.begin() + i);
					--i;
				}
				continue;

		// 		std::istringstream bufferString(buffer);
		// 		// setEnv(bufferString);
		// 		// printEnv();
		// 		std::map<std::string, envVars>::iterator it = _clientFeedback.find(METHOD);
		// 		if (it->second.first == GET)
		// 		{
		// 			if (!sendHTTPResponse(it->second.second, clientfd))
		// 			{
		// 				close(clientfd);
		// 				FD_CLR(clientfd, &master_set);
		// 				clientSockets.erase(clientSockets.begin() + i);
		// 				--i;
		// 				continue;
		// 			}
		// 		}
		// 		else if (it->second.first == POST)
		// 		{
		// 			if (!postResponse(it->second.second, clientfd))
		// 			{
		// 				close(clientfd);
		// 				FD_CLR(clientfd, &master_set);
		// 				clientSockets.erase(clientSockets.begin() + i);
		// 				--i;
		// 				continue;
		// 			}
		// 		}
			}
		}
		// iteratorClean();
	}
}

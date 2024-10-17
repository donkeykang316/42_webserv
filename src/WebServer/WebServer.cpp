#include "includes/WebServer.hpp"

WebServer::WebServer(ServerConfig &serverConfigElem, Dictionary &dictionaryGlobal):serverConfig(serverConfigElem), dictionary(dictionaryGlobal)
{
		for (std::set<std::string>::iterator portIt = serverConfig.listen.begin(); portIt != serverConfig.listen.end(); portIt++)
		{
			struct addrinfo hints = addrinfo();
			hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
			getaddrinfo(serverConfig.serverName.c_str(), (*portIt).c_str(), &(hints), &res);
			socket_fds[*portIt] = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
			fds[0].fd = socket_fds[*portIt];
			fds[0].events = POLLIN;
			bind(socket_fds[*portIt], res->ai_addr, res->ai_addrlen);
			listen(socket_fds[*portIt], BACKLOG);
			launch();
		}
}

WebServer::~WebServer()
{
}

void WebServer::launch()
{
	while (true)
	{
		std::cout << "---LAUNCH----" << std::endl;
		_accept();
		// _handle();
		// _respond();
		std::cout << "---LAUNCH STOPED----" << std::endl;

	}
}

void	WebServer::_accept()
{
	for(std::map<std::string, int>::iterator socketFd = socket_fds.begin(); socketFd != socket_fds.end(); socketFd++)
	{
		char buffer[30000];
		HTTPRequest *curr_request = NULL;
		newSocketFds[(*socketFd).first] = accept((*socketFd).second, (res->ai_addr), &(res->ai_addrlen));
		int newSocketFd = newSocketFds[(*socketFd).first];
		read(newSocketFd, buffer, 30000);
		curr_request = _handle(buffer);
		_respond(newSocketFd, curr_request);
	}
}

HTTPRequest	*WebServer::_handle( char * buffer)
{
	std::cout << buffer << std::endl;
	return (new HTTPRequest(buffer, dictionary));
}

void	WebServer::_respond(int newSocketFd, HTTPRequest *request)
{
	std::string responseFile = getResponseFilePath(request, serverConfig);
	HTTPResponse response(*request, responseFile);
	delete request;
	std::cout << "RESPONSE DATA: " << response.response << std::endl;
	send(newSocketFd,response.response.c_str(), response.response.size(),0);
	close(newSocketFd);
}

std::string WebServer::getResponseErrorFilePath(LocationConfig *location, enum status_code_value statusCode)
{

	if (!location)
		return ("");
	std::vector<std::string> errPathVector;
	std::string errPagePath = ".";
	switch (statusCode)
	{
	case forbidden:
		errPathVector =location->errorPages["403"];
		break;
	case not_found:
		errPathVector =location->errorPages["404"];
		break;
	case method_not_allowed:
		errPathVector =location->errorPages["405"];
		break;
	default:
		break;
	}
	if (!errPathVector.size())
		return ("");
	errPagePath.append(errPathVector[0]);
	errPagePath.append("/");
	errPagePath.append(errPathVector[1]);
	int fileFd = open(errPagePath.c_str(), O_RDONLY);
	if (fileFd < 0)
	{
		errPagePath.clear();
	}
	else
		close(fileFd);
	return (errPagePath);
}

std::string WebServer::getResponseFilePath(HTTPRequest *request, ServerConfig &serverConfig)
{
	std::string filePath;

	std::string requestUri = request->get_path();
	// get the location that matches uri of the request
	LocationConfig *location = serverConfig.getLocation(requestUri);
	// if there no location or method is not allowed set error status code
	if (location == NULL)
		request->setStatusCode(not_found);
	else if (!location->isMethodAllowed(request->get_method()))
		request->setStatusCode(method_not_allowed);
	// get path of file
	else
	{
		std::cout << "1. Location for path found  " << location->root << std::endl;

		std::string restUriPath;

		filePath.append(location->root);

		size_t requestPathSize = request->get_path().size();
		size_t restUriSize = requestPathSize - location->uri.size();
		if (restUriSize)
		{
			filePath.append("/");
			restUriPath.append(requestUri.substr(requestPathSize - restUriSize));
		}

		filePath.append(restUriPath);

		std::string fileExtention = getUriExtention(filePath);
		if (!fileExtention.size())
		{
			filePath.append("/");
			filePath.append(location->index);
		}
		std::string relativePath = ".";
		relativePath.append(filePath);
		int fileFd = open(relativePath.c_str(), O_RDONLY);
		if (fileFd < 0)
		{
			request->setStatusCode(not_found);
		}
		else
			close(fileFd);

		if (!request->get_status_code() || request->get_status_code() == ok)
			return (relativePath);
		else
		{
			filePath.clear();
			filePath.append(getResponseErrorFilePath(location, request->get_status_code()));
		}
	}
	return (filePath);
}


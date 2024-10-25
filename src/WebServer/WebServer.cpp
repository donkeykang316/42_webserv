#include "includes/WebServer.hpp"

WebServer::WebServer(ServerConfig *serverConfigElem, Dictionary &dictionaryGlobal):dictionary(dictionaryGlobal)
{
	serverConfig = serverConfigElem;
		// for (std::set<std::string>::iterator portIt = serverConfig.listen.begin(); portIt != serverConfig.listen.end(); portIt++)
		// {
		// 	struct addrinfo hints = addrinfo();
		// 	hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
		// 	hints.ai_socktype = SOCK_STREAM;
		// 	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
		// 	getaddrinfo(serverConfig.serverName.c_str(), (*portIt).c_str(), &(hints), &res);
		// 	socket_fds[*portIt] = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		// 	fds[0].fd = socket_fds[*portIt];
		// 	fds[0].events = POLLIN;
		// 	bind(socket_fds[*portIt], res->ai_addr, res->ai_addrlen);
		// 	listen(socket_fds[*portIt], BACKLOG);
		// 	launch();
		// }
}

	WebServer& WebServer::operator=(const WebServer &rhs)
	{
		(void) rhs;
		std::cout << "OPERATOR =!!!!!!" <<std::endl;
		return (*this);
	}

WebServer::~WebServer()
{
	// delete serverConfig;
}

// void WebServer::launch()
// {
// 	// while (true)
// 	// {
// 	// 	std::cout << "---LAUNCH----" << std::endl;
// 	// 	_accept();
// 	// 	// _handle();
// 	// 	// _respond();
// 	// 	std::cout << "---LAUNCH STOPED----" << std::endl;

// 	// }
// }

// void	WebServer::_accept()
// {
// 	// for(std::map<std::string, int>::iterator socketFd = socket_fds.begin(); socketFd != socket_fds.end(); socketFd++)
// 	// {
// 	// 	char buffer[30000];
// 	// 	HTTPRequest *curr_request = NULL;
// 	// 	newSocketFds[(*socketFd).first] = accept((*socketFd).second, (res->ai_addr), &(res->ai_addrlen));
// 	// 	int newSocketFd = newSocketFds[(*socketFd).first];
// 	// 	read(newSocketFd, buffer, 30000);
// 	// 	curr_request = _handle(buffer);
// 	// 	_respond(newSocketFd, curr_request);
// 	// }
// }

// HTTPRequest	*WebServer::_handle( char * buffer)
// {
// 	std::cout << buffer << std::endl;
// 	return (new HTTPRequest(buffer, dictionary));
// }

// void	WebServer::_respond(int newSocketFd, HTTPRequest *request)
// {
// 	std::string responseFile = getResponseFilePath(request, serverConfig);
// 	HTTPResponse response(*request, responseFile);
// 	delete request;
// 	std::cout << "RESPONSE DATA: " << response.response << std::endl;
// 	send(newSocketFd,response.response.c_str(), response.response.size(),0);
// 	close(newSocketFd);
// }

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

std::string WebServer::getRedirectionPath(std::string &requestUri, LocationConfig *location, HTTPRequest *request)
{
	std::string redirectionUri = "";

	//get location redirection
	if (location && !location->getRedirection().first.empty())
	{
		request->setRequestType(REDIRECT_REQUEST);
		redirectionUri.append(location->getRedirection().second);
		request->setStatusCode(moved_permanently);
	}
	std::pair<std::string, std::string> serverRedirection = serverConfig->getRedirection();
	// get server redirection
	std::string serverStatusCode = serverRedirection.first;
	if (request->getRequestType() != REDIRECT_REQUEST && !serverStatusCode.empty())
	{
		request->setRequestType(REDIRECT_REQUEST);
		redirectionUri.append(serverRedirection.second);
		request->setStatusCode(moved_permanently);
	}
	std::string confVarRedirUri = dictionary.getConfigVariable("REQUEST_URI");
	size_t confVarRedirUriSize = confVarRedirUri.size();
	if (confVarRedirUriSize)
	{
		size_t pos = redirectionUri.find(confVarRedirUri);
		if (pos == std::string::npos)
			return (redirectionUri);
		redirectionUri.replace(redirectionUri.begin() + pos, redirectionUri.end() + pos + confVarRedirUriSize, requestUri);
	}
	return (redirectionUri);
}


std::string WebServer::getResponseFilePath(HTTPRequest *request)
{
	std::string filePath;

	std::string requestUri = request->get_path();

		// get the location that matches uri of the request
	request->location = serverConfig->getLocation(requestUri);
	// check is it cgi
	if (request->location->isCgi)
		return (requestUri);

	// redirection
	filePath = getRedirectionPath(requestUri, request->location, request);
	if (request->getRequestType() == REDIRECT_REQUEST)
		return (filePath);

	// if there no location or method is not allowed set error status code
	if (request->location == NULL)
		request->setStatusCode(not_found);
	else if (!request->location->isMethodAllowed(request->get_method()))
		request->setStatusCode(method_not_allowed);
	// get path of file
	else
	{
		std::string restUriPath;

		filePath.append(request->location->root);

		size_t requestPathSize = request->get_path().size();
		size_t restUriSize = requestPathSize - request->location->uri.size();
		if (restUriSize)
		{
			filePath.append("/");
			restUriPath.append(requestUri.substr(requestPathSize - restUriSize));
		}

		filePath.append(restUriPath);

		std::string fileExtention = getUriExtention(filePath);
		if (request->location->autoindex && !fileExtention.size() && request->location->index.empty())
		{
			if (request->getRequestType() == GET_FILE)
				request->setRequestType(GET_DIR_LIST);
			DIR *dir;
	// struct dirent *ent;
			std::string relativePath = ".";
			relativePath.append(filePath);
			if ((dir = opendir(relativePath.c_str())) != NULL)
			{
				std::cout << "FILE DIR IS AVAILABLE " << filePath << std::endl;
				closedir (dir);
				return (filePath);
			}
			std::cout << "FILE DIR IS NOT AVAILABLE " << filePath << std::endl;

			closedir (dir);
			request->setStatusCode(not_found);
		}
		if (!fileExtention.size())
		{
			filePath.append("/");
			filePath.append(request->location->index);
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
			filePath.append(getResponseErrorFilePath(request->location, request->get_status_code()));
		}
	}
	return (filePath);
}

std::set<std::string> WebServer::getServerNameAliases()
{
	return (this->serverConfig->getServerNameAliases());
};


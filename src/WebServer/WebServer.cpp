#include "includes/WebServer.hpp"

WebServer::WebServer(ServerConfig *serverConfigElem, Dictionary &dictionaryGlobal):dictionary(dictionaryGlobal)
{
	serverConfig = serverConfigElem;
}

	WebServer& WebServer::operator=(const WebServer &rhs)
	{
		(void) rhs;
		std::cout << "OPERATOR =!!!!!!" <<std::endl;
		return (*this);
	}

WebServer::~WebServer()
{
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
	if (!request->location)
		return ("");
	if (request->location->isCgi)
	{
		return (requestUri);
	}

	// redirection
	filePath = getRedirectionPath(requestUri, request->location, request);
	if (request->getRequestType() == REDIRECT_REQUEST)
		return (filePath);

	// if there no location or method is not allowed set error status code
	if (request->location == NULL)
		request->setStatusCode(not_found);
	else if (!request->location->isMethodAllowed(request->get_method()))
		request->setStatusCode(method_not_allowed);
	// else if (request->getRequestType() == GET_FILE && request->location.)
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


#ifndef HTTPPRESPONSE_H
# define HTTPPRESPONSE_H

#include "HTTP.h"
#include "HTTPRequest.hpp"
#include <sstream>
#include <cstdio>
# include <string>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>



#include <sys/select.h>

#include "../../common/colors.h"

class HTTPRequest;

class HTTPResponse
{
private:
	int clientFd;
	int tubes[2];
	int cgiResponseFds[2];
	enum status_code_value status_code;
	HTTPRequest &_request;
	std::string status_line;
	std::string response_headers;
	std::string content;
	void	_set_content(std::string filePath);
	std::string scriptExec;
public:
	HTTPResponse(int clientFd, HTTPRequest &request, std::string filePath);
	~HTTPResponse();
	std::string response;
	bool isFulfilled;
	void	get();
	void sendResponse();
	std::string getDefaultErrorPageContent(enum status_code_value statusCode);
	std::string getDirectoryListing(std::string filePath, std::string location);
	void urlEncode(std::string &url);
	void runCGI(LocationConfig *location, HTTPRequest *request);
	void setRequestData(const char *buffer, ssize_t len);
	void getEnvVariables(char ***envp);
	void clearEnvVariables(char ***envp);
};

#endif

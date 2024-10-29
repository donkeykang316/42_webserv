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

class HTTPRequest;

class HTTPResponse
{
private:
	int clientFd;
	// int fdForCGIIncome;
	int cgiResponseFd;
	enum status_code_value status_code;
	HTTPRequest &_request;
	std::string status_line;
	std::string response_headers;
	std::string content;
	void	_set_content(std::string filePath);
	void sendResponse();
	// char *paramsCGIList[4];
	// char ***envp;
	int cgiResponseFds[2];
	std::string scriptExec;
public:
	HTTPResponse(int clientFd, HTTPRequest &request, std::string filePath);
	~HTTPResponse();
	std::string response;
	int tubes[2];
	bool isFulfilled;
	void	get();
	std::string getDefaultErrorPageContent(enum status_code_value statusCode);
	std::string getDirectoryListing(std::string filePath);
	void urlEncode(std::string &url);
	void runCGI(LocationConfig *location, HTTPRequest *request);
	void setRequestData(const char *buffer);
	void getEnvVariables(char ***envp);
	void clearEnvVariables(char ***envp);
};

#endif

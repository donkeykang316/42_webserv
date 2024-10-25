#pragma once
#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sstream>
#include <fstream>
#include <fcntl.h>
#include <map>
#include <vector>
#include <algorithm>
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
#include <iomanip>
#include "HTTPRequest.hpp"

class cgi {
private:
	//enum status_code_value status_code;
	HTTPRequest &_request;
	std::string _filePath;
	std::string status_line;
	std::string response_headers;
	std::string content;
	std::string _cgiResponse;
	std::string _cgiBody;

	std::vector<std::string>			_env;

	std::map<std::string, std::string>	_headers;

public:
	cgi(HTTPRequest &request, std::string filePath);
	~cgi();
	void cgiHandler();
	std::string getCGIresponse() { return _cgiResponse; }
	std::string getCGIbody() { return _cgiBody; }
	void envInit();
	void executeCGI(const std::string &filePath, const std::string &cmd);
};
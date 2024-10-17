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

class cgi {
private:
	std::vector<std::string>			_env;
	std::string							_text;

	std::map<std::string, std::string>	_headers;

public:
	void envInit();
	int createListenSocket(std::string &portInfo);
	void executeCGI(std::string &filePath, std::string &cmd);
	void resetHeaders();
	std::string getText();
};
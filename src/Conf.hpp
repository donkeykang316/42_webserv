#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <fstream>
#include <fcntl.h>
#include <map>
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

#define BACKLOG 10 

class Conf {
private:
	Conf() {}

public:
	struct addrinfo		_hints;
	const char*			_node; // e.g. "www.example.com" or IP
	const char*			_service;  // e.g. "http" or port number

	//Conf();
	~Conf();
};
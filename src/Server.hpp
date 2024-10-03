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

class Server {
private:
	int									_socketFD;
	int									_clientSocketFD;
	//struct sockaddr_in				_serverAddr;
	struct sockaddr_in					_clientAddr;
	std::string							_text;
	struct pollfd 						_fds[201];
	std::map<std::string, std::string>	_htmlFile;
	std::map<std::string, std::string>	_httpResponse;
	std::map<std::string, std::string>	_clientFeedback;
	std::map<int, std::string>			_fileExtension;

	Server() {}
	
public:
	Server(char** env);
	~Server();
	void fileExtensionInit();
	void getFile(std::string &filePath, struct stat fileStat);
};

//void sigInteruption(void);
//void sigHandler(int signum);

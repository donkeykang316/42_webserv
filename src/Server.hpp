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
#include <signal.h>
#include <errno.h>

class Server {
private:
	int									_socketFD;
	int									_clientSocketFD;
	//struct sockaddr_in					_serverAddr;
	struct sockaddr_in					_clientAddr;
	std::string							_text;
	std::map<std::string, std::string>	_htmlFile;
	std::map<std::string, std::string>	_httpResponse;
	std::map<std::string, std::string>	_clientFeedback;

	
public:
	Server();
	~Server();
	void getFile(std::string &file);
};

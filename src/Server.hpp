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
#include <iomanip>

#define METHOD "REQUEST_METHOD"
#define PROTOCOL "PROTOCOL_INFO"
#define IPINFO "IP_ADDRESS"
#define PORT "PORT_NBR"
#define GET "GET"
#define POST "POST"

class Server {
private:
	int											_socketFD;
	int											_clientSocketFD;
	struct sockaddr_in							_clientAddr;
	std::string									_text;
	struct pollfd 								_fds[201];
	std::map<std::string, std::string>			_htmlFile;
	std::map<std::string, std::string>			_httpResponse;
	std::map<int, std::string>					_fileExtension;
	typedef std::pair<std::string, std::string>	envVars;
	std::map<std::string, envVars>				_clientFeedback;
	
public:
	Server();
	Server(char** env);
	~Server();
	void setEnv(std::istringstream &bufferString);
	void fileExtensionInit();
	void getFile(std::string &filePath, struct stat fileStat);
	void sendHTTPResponse(std::string &method, char** env);
	void executeCGI(char** env, std::string &filePath, std::string cmd);
	void iteratorClean();

	void printEnv();
};

//void sigInteruption(void);
//void sigHandler(int signum);

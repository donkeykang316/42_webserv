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

#define METHOD "REQUEST_METHOD"
#define PROTOCOL "PROTOCOL_INFO"
#define IPINFO "IP_ADDRESS"
#define PORT "PORT_NBR"
#define GET "GET"
#define POST "POST"

class Server {
private:
	struct sockaddr_in							_clientAddr;
	std::string									_text;
	std::vector<int>							_listenSockets;
	std::map<std::string, std::string>			_htmlFile;
	typedef std::vector<std::string>			allFiles;
	std::map<std::string, allFiles>				_serverPath;
	std::map<std::string, std::string>			_httpResponse;
	std::map<int, std::string>					_fileExtension;
	typedef std::pair<std::string, std::string>	envVars;
	std::map<std::string, envVars>				_clientFeedback;
	
public:
	Server();
	~Server();
	void serverSetup();
	bool setNonBlocking(int sockfd);
	int createListenSocket(std::string &portInfo);
	void getAllFiles();
	void setEnv(std::istringstream &bufferString);
	void getFile(std::string &filePath, struct stat fileStat);
	bool sendHTTPResponse(std::string &method, int clientfd);
	bool postResponse(std::string &method, int clientfd);
	void executeCGI(std::string &filePath, std::string cmd);
	void iteratorClean();

	void printEnv();
	void printPathFile();
};

//void sigInteruption(void);
//void sigHandler(int signum);

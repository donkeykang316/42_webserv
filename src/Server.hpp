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
	void sendHTTPResponse(std::string &line, char** env);
	void executeCGI(char** env, std::string &filePath, std::string cmd);
};

/*void setupEnv(std::map<std::string, std::string>& envVars,
                    const std::string& method,
                    const std::string& queryString,
                    const std::string& contentLength,
                    const std::string& contentType,
                    const std::string& FileName,
                    const std::string& serverProtocol,
                    const std::string& remoteAddr,
                    const std::string& remotePort) {
    envVars["REQUEST_METHOD"] = method; // GET, POST, HEAD, PUT, DELETE example: REQUEST_METHOD=GET
    envVars["QUERY_STRING"] = queryString; // example: QUERY_STRING=name=John&age=30
    envVars["CONTENT_LENGTH"] = contentLength; // example: CONTENT_LENGTH=123
    envVars["CONTENT_TYPE"] = contentType; // example: CONTENT_TYPE=text/html
    envVars["FILE_NAME"] = FileName; // example: SCRIPT_FILENAME=/usr/local/apache/cgi-bin/script.cgi
    envVars["SERVER_PROTOCOL"] = serverProtocol; // example: SERVER_PROTOCOL=HTTP/1.1 or SERVER_PROTOCOL=CGI/1.1
    envVars["REMOTE_ADDR"] = remoteAddr; // example: REMOTE_ADDR=127.0.0.1
    envVars["REMOTE_PORT"] = remotePort; // example: REMOTE_PORT=5000
}*/

//void sigInteruption(void);
//void sigHandler(int signum);

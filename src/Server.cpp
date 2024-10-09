#include "Server.hpp"

Server::Server() {
	memset(&_clientAddr, 0 , sizeof(_clientAddr));
}

Server::Server(char** env) {
	struct addrinfo hints;
	struct addrinfo *res = NULL;
	struct addrinfo *rp = NULL;

	Server();
	_text.clear();
	memset(&hints, 0 , sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	const char *port = "5000";
	int status = getaddrinfo(NULL, port, &hints, &res);
	if (status != 0) {
		std::cerr << gai_strerror(status);
	}

	for (rp = res; rp != NULL; rp = rp->ai_next) {
		_socketFD = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (_socketFD < 0) {
			std::cerr << "socket ERROR\n";
		}

		int opt = 1;
		if (setsockopt(_socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
			std::cerr << "setsocket ERROR\n";
			close(_socketFD);
			break;
		}
	
		if (bind(_socketFD, rp->ai_addr, rp->ai_addrlen) == 0) {
			std::cout << "binding success\n";
			break;
		}
		close(_socketFD);
	}

	if (rp == NULL) {
		std::cerr << "bind failed\n";
		close(_socketFD);
	}

	freeaddrinfo(res);

	if (rp == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		std::cerr << "Bind failed\n";
		//exit
	}

	if (listen(_socketFD, 5) < 0) {
		std::cerr << "listen failed\n";
		close(_socketFD);
	}

	fileExtensionInit();

	// Initialize all client fds to -1 (unused)
	for (int i = 0; i < 201; ++i) {
		_fds[i].fd = -1;
		_fds[i].events = 0;
	}
	_fds[0].fd = _socketFD;
	_fds[0].events = POLLIN; //set event to ready to read
	while(1) {
		if (poll(_fds, 201, -1) == -1) {
			std::cerr << "poll error\n";
		}

		std::cout << "Server is listening on port " << port << std::endl;
		socklen_t clientAddrLen = sizeof(_clientAddr);
		_clientSocketFD = accept(_socketFD, (struct sockaddr *)&_clientAddr, &clientAddrLen);
		if (_clientSocketFD < 0) {
			std::cerr << "Could not connect with the client!\n";
		}

		for (int i = 1; i < 201; ++i) {
			if (_fds[i].fd == -1) {
				_fds[i].fd = _clientSocketFD;
				_fds[i].events = POLLIN;
			}
		}
		// Read client's request
    	char buffer[1024];
    	ssize_t bytesReceived = recv(_clientSocketFD, buffer, sizeof(buffer) - 1, 0);
    	if (bytesReceived == -1) {
        	std::cerr << "Failed to read request from client: " << strerror(errno) << "\n";
        	close(_clientSocketFD);
        	return;
    	}
		std::cout << "buffer:\n" << buffer << std::endl;

		std::istringstream bufferString(buffer);
		setEnv(bufferString);
		//printEnv();

		std::map<std::string, envVars>::iterator it = _clientFeedback.find(METHOD);
		if (it->second.first == GET) {
			std::cout << "GET recevied\n";
			sendHTTPResponse(it->second.second, env);
		}
		for (int i = 1; i < 201; ++i) {
			if (_fds[i].fd != -1) {
				close(_fds[i].fd);
			}
		}
		iteratorClean();
	}
}

Server::~Server() {
	close(_socketFD);
}

void Server::setEnv(std::istringstream &bufferString) {
	std::string	line;

	std::getline(bufferString, line);
	std::istringstream iss0(line);
	std::string method;
    std::string path;
    std::string protocol;
	iss0 >> method >> path >> protocol;
	std::string protocolName = protocol.substr(0, protocol.find('/'));
    std::string protocolVersion = protocol.substr(protocol.find('/') + 1);
	envVars env0;
	env0.first = method;
	env0.second = path;
	_clientFeedback["REQUEST_METHOD"] = env0;
	envVars env1;
	env1.first = protocolName;
	env1.second = protocolVersion;
	_clientFeedback["PROTOCOL_INFO"] = env1;

	std::getline(bufferString, line);
	std::istringstream iss1(line);
	std::string host;
    std::string IPPort;
	std::string	port = "Port";
	iss1 >> host >> IPPort;
	std::string hostNew = host.substr(0, host.length() - 1);
	std::string IP = IPPort.substr(0, IPPort.find(':'));
    std::string portNbr = IPPort.substr(IPPort.find(':') + 1);
	envVars env2;
	env2.first = hostNew;
	env2.second = IP;
	_clientFeedback["IP_ADDRESS"] = env2;
	envVars env3;
	env3.first = port;
	env3.second = portNbr;
	_clientFeedback["PORT_NBR"] = env3;

	int i = 0;
	while (std::getline(bufferString, line)) {
		envVars env00;
		std::stringstream ss;
		++i;
		ss << i;
		std::string nbr = ss.str();
		std::string key = line.substr(0, line.find(':'));
		env00.first = key;
		env00.second = line.substr(line.find(':') + 2, line.length());
		_clientFeedback[nbr] = env00;
	}
}

void Server::printEnv() {
	std::cout << "ENV:\n";
    for (std::map<std::string, envVars>::iterator It = _clientFeedback.begin(); It != _clientFeedback.end(); ++It) {
        std::cout << "	" << std::left << std::setw(30) << It->first;
        std::cout << "  " << std::left << std::setw(30) << It->second.first << " " << It->second.second;
		std::cout << std::endl;
    }
	std::cout << std::endl;
}

void Server::getFile(std::string &filePath, struct stat fileStat) {

	/*std::string filePath = "server/" + file + _fileExtension[0];
    if (stat(filePath.c_str(), &fileStat) == -1) {
		std::string filePath = "server/" + file + _fileExtension[1];
		std::cout << "file path:" << filePath << std::endl;
		if (stat(filePath.c_str(), &fileStat) == -1) {
    		std::cerr << "stat error\n";
		}
	}*/

	// Check if the entry is a regular file
	if (S_ISREG(fileStat.st_mode)) {
    	// Open the file
    	FILE *fileToRead = std::fopen(filePath.c_str(), "r");
    	if (fileToRead == NULL) {
        	std::cerr << "file open error\n";
    	}

    	// Print the name of the file
		std::cout << "Contents of " << filePath << ":\n"; 

        // Read and print the contents of the file
        int c;
        while ((c = std::fgetc(fileToRead)) != EOF) {
			char ch = c;
        	std::stringstream ss;
    		ss << ch;
			_text += ss.str();
        }

    	// Close the file
    	std::fclose(fileToRead);
		std::cout << "\n\n"; // Add spacing between files 
	}
}

void Server::sendHTTPResponse(std::string &method, char **env) {
	std::cout << "metod: " << method << std::endl;

	std::string fileAccess = method;

	std::cout << "fileAccess: " << fileAccess << std::endl;

	struct stat fileStat;
	if (stat(("server/" + fileAccess + _fileExtension[0]).c_str(), &fileStat) != -1) {
		std::string filePath = "server/" + fileAccess + _fileExtension[0];
		std::cout << "html: " << filePath << std::endl;
		getFile(filePath, fileStat);
	} else if (stat(("server/" + fileAccess + _fileExtension[1]).c_str(), &fileStat) != -1) {
		std::string filePath = "server/" + fileAccess + _fileExtension[1];
		std::cout << "php: " << filePath << std::endl;
		executeCGI(env, filePath, "php");
	} else if (stat(("server/" + fileAccess + _fileExtension[2]).c_str(), &fileStat) != -1) {
		std::string filePath = "server/" + fileAccess + _fileExtension[2];
		std::cout << "py: " << filePath << std::endl;
		executeCGI(env, filePath, "python3");
	}

	size_t cLen = _text.size();
	std::stringstream contentLen;
    contentLen << cLen;
	std::string cL = contentLen.str();
	std::cout << cL << std::endl;
	// Prepare an HTTP response
	std::string httpResponse = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: text/html\r\nContent-Length: " 
		+ cL + "\r\n"
		+ "\r\n"
		+ _text;

	if (send(_clientSocketFD, httpResponse.c_str(), httpResponse.size(), 0) < 0) {
    	std::cerr << "Failed to send response to the client.\n";
	}
}


void Server::fileExtensionInit() {
	int key = 0;
	_fileExtension[key] = ".html";
	++key;
	_fileExtension[key] = ".php";
	++key;
	_fileExtension[key] = ".py";
}

void Server::executeCGI(char **env, std::string &filePath, std::string cmd) {
	int inPipe[2];
    int outPipe[2];

	if (pipe(inPipe) == -1 || pipe(outPipe) == -1) {
		std::cerr << "Failed to create pipes.\n";
		return;
	}

	pid_t pid = fork();
	if (pid == -1) {
		std::cerr << "Failed to fork.\n";
		return;
	}

	if (pid == 0) {
		// Child process
		close(inPipe[1]);
		close(outPipe[0]);

		// Redirect stdin and stdout
		dup2(inPipe[0], STDIN_FILENO);
		dup2(outPipe[1], STDOUT_FILENO);

		std::string cmdPath = "/bin/" + cmd;
    	char *const argv[] = { const_cast<char*>(cmd.c_str()), const_cast<char*>(filePath.c_str()), NULL };
		std::cerr << "line: " << filePath << std::endl;

		// Execute the CGI script
		if (execve(cmdPath.c_str(), argv, env) == -1) {
			std::cerr << "Execution failed!\n";
			std::exit(1);
		}
		std::exit(1);
	}

	int status;
    if (waitpid(pid, &status, 0) == -1) {
        std::cerr << "waitpid failed: " << strerror(errno) << std::endl;
            return ;
    }
    if (WIFEXITED(status)) {
        std::cout << "Child exited with status " << WEXITSTATUS(status) << std::endl;
    }
    else if (WIFSIGNALED(status)) {
        std::cout << "Child killed by signal " << WTERMSIG(status) << std::endl;
    }

	close(outPipe[1]);

	char buffer[1024];
    ssize_t bytesRead;
    while ((bytesRead = read(outPipe[0], buffer, sizeof(buffer))) > 0) {
            _text.append(buffer, bytesRead);
    }
	close(outPipe[0]);
    
}

void Server::iteratorClean() {
	_text.clear();
	for (std::map<std::string, envVars>::iterator it = _clientFeedback.begin(); it != _clientFeedback.end(); ++it) {
		it->second.first.clear();
		it->second.second.clear();
	}
	_clientFeedback.clear();
}

/*void sigInteruption(void)
{
	struct sigaction	sa;

	std::memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &sigHandler;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGINT);
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}

void	sigHandler(int signum)
{
	if (signum == SIGINT){
		std::cerr << "Signal interuption\n";
	}
}*/
#include "Server.hpp"

Server::Server() {
	std::cout << "server default constructor\n";
}

Server::Server(char** env) {
	serverSetup();
	/*struct addrinfo hints;
	struct addrinfo *res = NULL;
	struct addrinfo *rp = NULL;
	

	memset(&hints, 0 , sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;*/

	/*const char* port = static_cast<const char*>(_port.c_str());
	int status = getaddrinfo(NULL, port, &hints, &res);
	if (status != 0) {
		std::cerr << gai_strerror(status);
	}*/

	// will be replaced with config parser
	std::vector<std::string> ports;
	ports.push_back("5000");
	ports.push_back("7000");
	ports.push_back("8000");
	ports.push_back("8080");
	ports.push_back("9000");

	// Create listening sockets for each port
    for (size_t i = 0; i < ports.size(); ++i) {
        int sock = createListenSocket(ports[i]);
        _listenSockets.push_back(sock);
    }

	// Set up the master file descriptor set
    fd_set master_set, read_fds;
    FD_ZERO(&master_set);
    int fd_max = 0;

	// Add listening sockets to the master set
    for (size_t i = 0; i < _listenSockets.size(); ++i) {
        FD_SET(_listenSockets[i], &master_set);
        if (_listenSockets[i] > fd_max) {
            fd_max = _listenSockets[i];
        }
    }

	// Vector to keep track of client sockets
    std::vector<int> clientSockets;
	while(1) {
		read_fds = master_set;

		int activity = select(fd_max + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0) {
            if (errno == EINTR) continue; // Interrupted by signal
            std::cerr << "select" << errno << std::endl;
            break;
        }

		// Iterate through listening sockets to check for new connections
        for (size_t i = 0; i < _listenSockets.size(); ++i) {
			int sockfd = _listenSockets[i];
			if (FD_ISSET(sockfd, &read_fds)) {
				struct sockaddr_in client_addr;
                socklen_t addrlen = sizeof(client_addr);
                int newfd = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
                if (newfd < 0) {
                    if (errno != EWOULDBLOCK && errno != EAGAIN) {
						std::cerr << "accept" << errno << std::endl;
                    }
                    continue;
                }
				if (!setNonBlocking(newfd)) {
					std::cerr << "fcntl" << errno << std::endl;
                    close(newfd);
                    continue;
                }

				// Add the new socket to the master set and client list
                FD_SET(newfd, &master_set);
                if (newfd > fd_max) fd_max = newfd;
                clientSockets.push_back(newfd);
			}
		}

		// Iterate through client sockets to check for incoming data
        for (size_t i = 0; i < clientSockets.size(); ++i) {
			int clientfd = clientSockets[i];
            if (FD_ISSET(clientfd, &read_fds)) {
				// Read client's request
				char buffer[1024];
				ssize_t bytesReceived = recv(clientfd, buffer, sizeof(buffer) - 1, 0);
				if (bytesReceived == -1) {
					std::cerr << "Failed to read request from client: " << strerror(errno) << "\n";
					close(clientfd);
                	FD_CLR(clientfd, &master_set);
                	// Remove from clientSockets
                	clientSockets.erase(clientSockets.begin() + i);
                	--i; // Adjust index after removal
                	continue;
				}
				std::cout << "buffer:\n" << buffer << std::endl;
				std::istringstream bufferString(buffer);
				setEnv(bufferString);
				//printEnv();
				std::map<std::string, envVars>::iterator it = _clientFeedback.find(METHOD);
				if (it->second.first == GET) {
					if (!sendHTTPResponse(it->second.second, clientfd, env)) {
                    	close(clientfd);
                		FD_CLR(clientfd, &master_set);
                    	clientSockets.erase(clientSockets.begin() + i);
                    	--i;
                    	continue;
					}
				}
			}
		}

		/*std::cout << "Server is listening on port " << _port << std::endl;
		socklen_t clientAddrLen = sizeof(_clientAddr);
		_clientSocketFD = accept(_listenSockets[0], (struct sockaddr *)&_clientAddr, &clientAddrLen);
		if (_clientSocketFD < 0) {
			std::cerr << "Could not connect with the client!\n";
		}

		for (int i = 1; i < 201; ++i) {
			if (_clientfds[i].fd == -1) {
				_clientfds[i].fd = _clientSocketFD;
				_clientfds[i].events = POLLIN;
			}
		}
		// Read client's request
    	char buffer[1024];
    	ssize_t bytesReceived = recv(_clientSocketFD, buffer, sizeof(buffer) - 1, 0);
    	if (bytesReceived == -1) {
        	std::cerr << "Failed to read request from client: " << strerror(errno) << "\n";
        	close(_clientSocketFD);
    	}
		std::cout << "buffer:\n" << buffer << std::endl;

		std::istringstream bufferString(buffer);
		setEnv(bufferString);
		//printEnv();

		std::map<std::string, envVars>::iterator it = _clientFeedback.find(METHOD);
		if (it->second.first == GET) {
			sendHTTPResponse(it->second.second, env);
		}
		for (int i = 1; i < 201; ++i) {
			if (_clientfds[i].fd != -1) {
				close(_clientfds[i].fd);
			}
		}*/
		iteratorClean();
	}
}

Server::~Server() {}

void Server::serverSetup() {
	_port = "5000";
	memset(&_clientAddr, 0 , sizeof(_clientAddr));
	getAllFiles();
	//printPathFile();
}

bool Server::setNonBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1)
		return false;
    flags |= O_NONBLOCK;
    return (fcntl(sockfd, F_SETFL, flags) != -1);
}

int Server::createListenSocket(std::string &portInfo) {
    int sockfd = -1; // Initialize to -1 to indicate invalid socket
    struct addrinfo hints;
    struct addrinfo *res = NULL;
    struct addrinfo *rp = NULL;

    // Initialize hints before calling getaddrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;      // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;  // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;      // For wildcard IP address

    const char* port = portInfo.c_str(); // No need for static_cast

    // Call getaddrinfo with initialized hints
    int status = getaddrinfo(NULL, port, &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return -1; // Indicate failure
    }

    // Iterate through the address list and try to bind
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd < 0) {
            std::cerr << "socket creation failed: " << strerror(errno) << std::endl;
            continue; // Try the next address
        }

        // Set SO_REUSEADDR to allow reuse of local addresses
        int opt = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            std::cerr << "setsockopt SO_REUSEADDR failed: " << strerror(errno) << std::endl;
            close(sockfd);
            sockfd = -1;
            continue; // Try the next address
        }

        // Attempt to bind the socket
        if (bind(sockfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            std::cout << "Binding successful on port " << port << std::endl;
            break; // Successfully bound
        }

        std::cerr << "bind failed: " << strerror(errno) << std::endl;
        close(sockfd);
        sockfd = -1; // Reset sockfd
    }

    freeaddrinfo(res);

    if (rp == NULL) { // No address succeeded
        std::cerr << "Server: failed to bind to any address on port " << port << std::endl;
        return -1; // Indicate failure
    }

    // Start listening on the bound socket
    if (listen(sockfd, SOMAXCONN) < 0) {
        std::cerr << "listen failed: " << strerror(errno) << std::endl;
        close(sockfd);
        return -1; // Indicate failure
    }

    std::cout << "Server is listening on port " << port << std::endl;
    return sockfd; // Return the listening socket file descriptor
}


void Server::getAllFiles() {
	//will be replace with location
	std::vector<std::string> locationPath;
	locationPath.push_back("www/");
	locationPath.push_back("www/cgi_bin/");
	locationPath.push_back("www/error_pages/");
	locationPath.push_back("www/subpage/");

    for (std::vector<std::string>::iterator locit = locationPath.begin(); locit != locationPath.end(); ++locit) {
        allFiles fileNames;

        // Open the directory
        DIR* dir = opendir(locit->c_str());
        if (dir == NULL) {
            std::cerr << "Error: Could not open directory " << *locit << " - " << strerror(errno) << std::endl;
            continue;  // Skip to the next directory
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            std::string name = entry->d_name;

            // Skip current and parent directories
            if (name == "." || name == "..")
                continue;

            // Construct the full path for further processing if needed
            std::string fullPath = *locit + name;

            // Optionally, check if the entry is a file or directory
            struct stat info;
            if (stat(fullPath.c_str(), &info) != 0) {
                std::cerr << "Error: Could not get information for " << fullPath << " - " << strerror(errno) << std::endl;
                continue;
            }

            /*if (S_ISDIR(info.st_mode)) {
                std::cout << "[DIR]  " << name << std::endl;
            } else if (S_ISREG(info.st_mode)) {
                std::cout << "[FILE] " << name << std::endl;
            } else {
                std::cout << "[OTHER] " << name << std::endl;
            }*/

            fileNames.push_back("/" + name);  // Add the file/directory name to the vector
        }

        closedir(dir);
        _serverPath[*locit] = fileNames;  // Assign the collected file names to the map
    }
}

void Server::setEnv(std::istringstream &bufferString) {
    std::string line;

    // 1. Parse the Request Line
    if (!std::getline(bufferString, line)) {
        std::cerr << "Error: Empty request or failed to read request line.\n";
        return; // Or throw an exception
    }

    std::istringstream requestLineStream(line);
    std::string method, path, protocol;

    if (!(requestLineStream >> method >> path >> protocol)) {
        std::cerr << "Error: Malformed request line.\n";
        return; // Or throw an exception
    }

    // Validate and parse protocol
    size_t slashPos = protocol.find('/');
    if (slashPos != std::string::npos && slashPos + 1 < protocol.size()) {
        std::string protocolName = protocol.substr(0, slashPos);
        std::string protocolVersion = protocol.substr(slashPos + 1);

        // Store Request Method and Path
        envVars requestEnv;
        requestEnv.first = method;
        requestEnv.second = path;
        _clientFeedback["REQUEST_METHOD"] = requestEnv;

        // Store Protocol Information
        envVars protocolEnv;
        protocolEnv.first = protocolName;
        protocolEnv.second = protocolVersion;
        _clientFeedback["PROTOCOL_INFO"] = protocolEnv;
    } else {
        std::cerr << "Error: Invalid protocol format in request line.\n";
        return; // Or throw an exception
    }

    // 2. Parse the Host Header
    if (!std::getline(bufferString, line)) {
        std::cerr << "Error: Missing Host header.\n";
        return; // Or throw an exception
    }

    size_t colonPos = line.find(':');
    if (colonPos == std::string::npos) {
        std::cerr << "Error: Malformed Host header.\n";
        return; // Or throw an exception
    }

    std::string headerName = line.substr(0, colonPos);
    // Trim whitespace from headerName
    headerName.erase(headerName.find_last_not_of(" \t\r\n") + 1);
    headerName.erase(0, headerName.find_first_not_of(" \t\r\n"));

    if (headerName != "Host") {
        std::cerr << "Error: Expected Host header, found: " << headerName << "\n";
        return; // Or handle differently
    }

    std::string hostValue = line.substr(colonPos + 1);
    // Trim whitespace from hostValue
    size_t start = hostValue.find_first_not_of(" \t\r\n");
    size_t end = hostValue.find_last_not_of(" \t\r\n");
    if (start == std::string::npos || end == std::string::npos) {
        std::cerr << "Error: Empty Host header value.\n";
        return; // Or throw an exception
    }
    hostValue = hostValue.substr(start, end - start + 1);

    // Split hostValue into IP (or domain) and Port
    size_t portPos = hostValue.find_last_of(':');
    if (portPos != std::string::npos && portPos + 1 < hostValue.size()) {
        std::string ipOrDomain = hostValue.substr(0, portPos);
        std::string portNbr = hostValue.substr(portPos + 1);

        // Store IP Address or Domain
        envVars ipEnv;
        ipEnv.first = "Host";
        ipEnv.second = ipOrDomain;
        _clientFeedback["IP_ADDRESS"] = ipEnv;

        // Store Port Number
        envVars portEnv;
        portEnv.first = "Port";
        portEnv.second = portNbr;
        _clientFeedback["PORT_NBR"] = portEnv;
    } else {
        std::cerr << "Error: Host header missing port or malformed.\n";
        return; // Or throw an exception
    }

    // 3. Parse Additional Headers
    int headerCount = 0;
    while (std::getline(bufferString, line)) {
        // Skip empty lines
        if (line.empty() || line == "\r") {
            continue;
        }

        size_t sepPos = line.find(':');
        if (sepPos == std::string::npos || sepPos + 1 >= line.size()) {
            std::cerr << "Warning: Malformed header line: " << line << "\n";
            continue; // Skip or handle differently
        }

        std::string key = line.substr(0, sepPos);
        // Trim whitespace from key
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        key.erase(0, key.find_first_not_of(" \t\r\n"));

        std::string value = line.substr(sepPos + 1);
        // Trim whitespace from value
        size_t valStart = value.find_first_not_of(" \t\r\n");
        size_t valEnd = value.find_last_not_of(" \t\r\n");
        if (valStart == std::string::npos || valEnd == std::string::npos) {
            value = "";
        } else {
            value = value.substr(valStart, valEnd - valStart + 1);
        }

        // Store headers with their actual names
        envVars headerEnv;
        headerEnv.first = key;
        headerEnv.second = value;
        _clientFeedback[key] = headerEnv;

        ++headerCount;
    }

    // Log the number of headers parsed
    std::cout << "Parsed " << headerCount << " additional headers.\n";
}

void Server::getFile(std::string &filePath, struct stat fileStat) {
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

bool Server::sendHTTPResponse(std::string &method, int clientfd, char **env) {
	std::string fileAccess = method;
	(void)env;

	/*struct stat fileStat;
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
	}*/

	for (std::map<std::string, allFiles>::iterator it = _serverPath.begin(); it != _serverPath.end(); ++it) {
		struct stat fileStat;
        for (size_t i = 0; i < it->second.size(); ++i) {
			if (std::strncmp(fileAccess.c_str(), it->second[i].c_str(), fileAccess.length()) == 0
				&& stat((it->first + it->second[i]).c_str(), &fileStat) != -1) {
				std::string filePath = it->first + it->second[i];
				getFile(filePath, fileStat);
			}	
        }
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

	if (send(clientfd, httpResponse.c_str(), httpResponse.size(), 0) < 0) {
    	std::cerr << "Failed to send response to the client.\n";
		return false;
	}
	return true;
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

void Server::printEnv() {
	std::cout << "ENV:\n";
    for (std::map<std::string, envVars>::iterator It = _clientFeedback.begin(); It != _clientFeedback.end(); ++It) {
        std::cout << "	" << std::left << std::setw(30) << It->first;
        std::cout << "  " << std::left << std::setw(30) << It->second.first << " " << It->second.second;
		std::cout << std::endl;
    }
	std::cout << std::endl;
}

void Server::printPathFile() {
	std::cout << "\nServer Path Map Contents:\n";
    for (std::map<std::string, allFiles>::iterator it = _serverPath.begin(); it != _serverPath.end(); ++it) {
        std::cout << "Directory: " << it->first << std::endl;
        for (size_t i = 0; i < it->second.size(); ++i) {
            std::cout << "  - " << it->second[i] << std::endl;
        }
    }
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
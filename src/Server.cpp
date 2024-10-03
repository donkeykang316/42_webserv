#include "Server.hpp"

Server::Server() {}

Server::Server(char** env) {
	struct addrinfo hints;
	struct addrinfo *res;
	struct addrinfo *rp;

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

	std::cout << "Server is listening on port " << port << std::endl;
	while (1) {
		socklen_t clientAddrLen = sizeof(_clientAddr);
		_clientSocketFD = accept(_socketFD, (struct sockaddr *)&_clientAddr, &clientAddrLen);
		if (_clientSocketFD < 0) {
			std::cerr << "Could not connect with the client!\n";
		}

		// Read client's request
    	char buffer[1024];
    	ssize_t bytesReceived = recv(_clientSocketFD, buffer, sizeof(buffer) - 1, 0);
    	if (bytesReceived < 0) {
        	std::cerr << "Failed to read request from client: " << strerror(errno) << "\n";
        	close(_clientSocketFD);
        	return;
    	}
		std::cout << "buffer:\n" << buffer << std::endl;
		//std::cout << "byereceived: " << bytesReceived << std::endl;
		std::string	line;
		std::istringstream bufferString(buffer);
		std::getline(bufferString, line);
		std::string tmp = line.substr(line.find(' ') + 1);
		std::string fileAccess = tmp.substr(1, tmp.find(' ') - 1);
		//std::cout << "filE" << fileAcces << "EOF" <<std::endl;
		/*while (std::getline(bufferString, line)) {
        	std::string key = line.substr(0, line.find(':'));
        	std::string info = line.substr(line.find(':') + 2);
        	_clientFeedback[key] = info;
    	}*/
		if (!fork()) {
			struct stat fileStat;
			std::string filePath = "server/" + fileAccess + _fileExtension[0];
    		if (stat(filePath.c_str(), &fileStat) != -1) {
				getFile(filePath, fileStat);
			} else {
				std::string filePath = "server/" + fileAccess + _fileExtension[1];
				std::cout << "file path:" << filePath << std::endl;
				const char path[] = "/bin/php";
				const char *args[] = { "php", "home/kaan/Documents/42_webserv/test.php", NULL };
				if (execve(path, const_cast<char* const*>(args), env) == -1) {
    				std::cerr << "stat error\n";
				}
				std::cerr << "stat error\n";
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
			std::exit(0);
    	}
		close(_clientSocketFD);
	}
}

Server::~Server() {
	close(_socketFD);
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

void Server::fileExtensionInit() {
	int key = 0;
	_fileExtension[key] = ".html";
	++key;
	_fileExtension[key] = ".php";
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
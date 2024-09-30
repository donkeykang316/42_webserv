#include "Server.hpp"

Server::Server() {
	struct addrinfo hints;
	struct addrinfo *res;
	struct addrinfo *rp;

	memset(&hints, 0 , sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_flags = AI_PASSIVE;

	const char *port = "5000";
	int status = getaddrinfo(NULL, port, &hints, &res);
	if (status != 0) {
		std::cerr << gai_strerror(status);
	}

	for (rp = res; rp != NULL; rp = rp->ai_next) {
		_socketFD = socket(AF_INET, SOCK_STREAM, 0);
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

	if (listen(_socketFD, 5) < 0) {
		std::cerr << "listen failed\n";
		close(_socketFD);
	}

	serverInit();

	std::cout << "Server is listening on port " << port << std::endl;
	while (1)
	{
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
    	buffer[bytesReceived] = '\0'; // Null-terminate the buffer
		//std::cout << "buffer: " << buffer << std::endl;
		std::string	line;
		std::istringstream bufferString(buffer);
		std::getline(bufferString, line);
		while (std::getline(bufferString, line)) {
        	std::string key = line.substr(0, line.find(':'));
        	std::string info = line.substr(line.find(':') + 2);
        	_clientFeedback[key] = info;
    	}
    	//std::cout << "Received request: " << _clientFeedback["Referer"] << std::endl;
		//std::cout << "Received request: " << _clientFeedback["Host"] << std::endl;
		std::string httpAdrr = "http://" + _clientFeedback["Host"];
		std::cout << "httpadd: " << httpAdrr << std::endl;
		std::string path = _clientFeedback["Referer"].substr(0, _clientFeedback["Referer"].find(httpAdrr));
		std::cout << "Path: " << path << std::endl;


		// Prepare an HTTP response
		std::string httpResponse = "HTTP/1.1 200 OK\n"
			"\n"
			+ _htmlFile["server/index.html"];

    	if (send(_clientSocketFD, httpResponse.c_str(), httpResponse.size(), 0) < 0) {
        	std::cerr << "Failed to send response to the client.\n";
    	}
		close(_clientSocketFD);
	}
	freeaddrinfo(res);
}

Server::~Server() {
	close(_socketFD);
}

void Server::serverInit() {
	std::string key = "server/index.html";
	std::ifstream file;
	file.open(key.c_str());
	if (!file.is_open()) {
        throw std::exception();
    }
	std::string line;
    while (std::getline(file, line)) {
		_htmlFile[key] += line + "\n";
	}
}

/*std::string Server::getFile() {
	DIR *dirp;
    struct dirent *entry;

    // Change the current working directory to the specified path
    if (chdir(av[1]) == -1) {
        std::cerr << "file doesn't exist\n";
        //exit
    }

    // Open the current directory
    dirp = opendir(".");
    if (dirp == NULL) {
        std::cerr << "Open file error\n";
        //exit
    }

    // Read directory entries
    while ((entry = readdir(dirp)) != NULL) {
        // Skip the "." and ".." entries
        if (std::strcmp(entry->d_name, ".") == 0 || std::strcmp(entry->d_name, "..") == 0)
            continue;

        // Get file information using stat()
        struct stat fileStat;
        if (stat(entry->d_name, &fileStat) == -1) {
            std::cerr << "stat error\n";
            continue;
        }

        // Check if the entry is a regular file
        if (S_ISREG(fileStat.st_mode)) {
            // Open the file
            FILE *file = std::fopen(entry->d_name, "r");
            if (file == NULL) {
                std::cerr << "file open error\n";
                continue;
            }

            // Print the name of the file
            printf("Contents of %s:\n", entry->d_name);
			std::cout << "Contents of " << entry->d_name << ":\n"; 

            // Read and print the contents of the file
            int c;
            while ((c = std::fgetc(file)) != EOF) {
                std::putchar(c);
            }

            // Close the file
            std::fclose(file);
			std::cout << "\n\n"; // Add spacing between files 
        }
    }

    // Check for errors after the loop
    if (errno != 0 && !entry) {
        perror("readdir");
        closedir(dirp);
        //exit
    }

    // Close the directory stream
    if (closedir(dirp) == -1) {
        perror("closedir");
        //exit
    }
}*/
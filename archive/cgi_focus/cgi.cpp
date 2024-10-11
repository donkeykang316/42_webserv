#include "cgi.hpp"

int createListenSocket(std::string &portInfo) {
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

void executeCGI(std::string _text, std::string &filePath, std::string cmd) {
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
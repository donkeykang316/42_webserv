#include "includes/ServerSocket.hpp"

ServerSocket::ServerSocket(std::string &port, WebServer *server)
{
    socketFd = createServerSocket(port);
    if (socketFd < 0)
        return;
    servers.push_back(server);
    std::set<std::string> serverNameAliases = server->getServerNameAliases();
    for (std::set<std::string>::iterator it = serverNameAliases.begin(); it != serverNameAliases.end(); it++)
    {
        if (hostToServer.find(*it) == hostToServer.end())
            hostToServer[*it] = server;
    }
    std::cout << "SOCKET CREATED " << socketFd << std::endl;
}

ServerSocket::~ServerSocket()
{
}

void ServerSocket::updateServerSocket(WebServer *server)
{
    servers.push_back(server);
    std::set<std::string> serverNameAliases = server->getServerNameAliases();
    for (std::set<std::string>::iterator it = serverNameAliases.begin(); it != serverNameAliases.end(); it++)
    {
        if (hostToServer.find(*it) == hostToServer.end())
            hostToServer[*it] = server;
    }
}
int ServerSocket::createServerSocket(std::string portInfo)
{

    int sockfd = -1; // Initialize to -1 to indicate invalid socket
    struct addrinfo hints = addrinfo();
    struct addrinfo *res = NULL;
    struct addrinfo *rp = NULL;

    // Initialize hints before calling getaddrinfo
    hints.ai_family = AF_UNSPEC;     // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_protocol = 0; // TCP protocol
    hints.ai_flags = AI_PASSIVE;     // For wildcard IP address

    const char *port = portInfo.c_str(); // No need for static_cast

    // Call getaddrinfo with initialized hints
    int status = getaddrinfo(NULL, port, &hints, &res);
    if (status != 0)
    {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return -1; // Indicate failure
    }

    // Iterate through the address list and try to bind
    for (rp = res; rp != NULL; rp = rp->ai_next)
    {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd < 0)
        {
            std::cerr << "socket creation failed: " << strerror(errno) << std::endl;
            continue; // Try the next address
        }
        fcntl(sockfd, F_SETFL, O_NONBLOCK);
        // Set SO_REUSEADDR to allow reuse of local addresses
        int opt = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        {
            std::cerr << "setsockopt SO_REUSEADDR failed: " << strerror(errno) << std::endl;
            close(sockfd);
            sockfd = -1;
            continue; // Try the next address
        }

        // Attempt to bind the socket
        if (bind(sockfd, rp->ai_addr, rp->ai_addrlen) == 0)
        {
            std::cout << "Binding successful on port " << port << std::endl;
            break; // Successfully bound
        }

        std::cerr << "bind failed: " << strerror(errno) << std::endl;
        close(sockfd);
        sockfd = -1; // Reset sockfd
    }

    freeaddrinfo(res);

    if (rp == NULL)
    { // No address succeeded
        std::cerr << "Server: failed to bind to any address on port " << port << std::endl;
        return -1; // Indicate failure
    }

    // Start listening on the bound socket
    if (listen(sockfd, SOMAXCONN) < 0)
    {
        std::cerr << "listen failed: " << strerror(errno) << std::endl;
        close(sockfd);
        return -1; // Indicate failure
    }

    std::cout << "Server is listening on port " << port << std::endl;
    return sockfd; // Return the listening socket file descriptor
}

int ServerSocket::getFd()
{
    return (this->socketFd);
}

WebServer *ServerSocket::getServer(std::string host)
{
    WebServer *server = this->hostToServer[host];
    if (server)
        return (server);
    return (servers[0]);
}

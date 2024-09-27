#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

class Server {
private:
	int					_socketFD;
	int					_clientSocketFD;
	/*
	struct sockaddr_in {
		sa_family_t    sin_family; // Address family (AF_INET)
    	in_port_t      sin_port;   // Port number (in network byte order)
    	struct in_addr sin_addr;   // IPv4 address
    	unsigned char  sin_zero[8]; // Padding to make structure the same size as `struct sockaddr`
	};
	Represents an IPv4 socket address

	struct sockaddr_in6 {
    	sa_family_t     sin6_family;   // Address family (AF_INET6)
		in_port_t       sin6_port;     // Port number (network byte order)
    	uint32_t        sin6_flowinfo; // IPv6 flow information
    	struct in6_addr sin6_addr;     // IPv6 address
    	uint32_t        sin6_scope_id; // Scope ID
	};
	Represents an IPv6 socket address
	*/
	struct sockaddr_in	_serverAddr;
	struct sockaddr_in	_clientAddr;
	
public:
	Server();
	~Server();
};
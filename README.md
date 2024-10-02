## Understand the Basics

### OSI || TCP/IP model
- Layer 3 (Network Layer): `AF_INET`: Deals with IPv4 addressing and routing.
- Layer 4 (Transport Layer): `SOCK_STREAM`: Uses TCP for reliable, connection-oriented communication.
- Layer 7 (Application Layer): HTTP Protocol: The actual data being transmitted (e.g., HTTP requests and responses).

### Socket Programming
- Familiarize yourself with TCP/IP socket programming, as HTTP operates over TCP [Socket Programming tutorial](https://www.youtube.com/watch?v=bqj4dWG7v3c&list=PLhnN2F9NiVmAMn9iGB_Rtjs3aGef3GpSm)

- basic server:
```javascript
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
/*
socket funtion breakdown:
	Domain value: 
		AF_INET: IPv4 Internet protocols
		AF_INET6: IPv6 Internet protocols
		AF_UNIX or AF_LOCAL: Local communication (on the same machine)
	Type value:
		SOCK_STREAM: Provides sequenced, reliable, two-way, connection-based byte streams. Corresponds to the TCP protocol
		SOCK_RAW: Provides raw network protocol access
*/
_socketFD = socket(AF_INET, SOCK_STREAM, 0);
if (_socketFD < 0) {
	std::cerr << "socket ERROR\n";
}

_serverAddr.sin_family = AF_INET;
_serverAddr.sin_port = htons(5000);
/*
struct in_addr {
	uint32_t s_addr; // 32-bit IPv4 address (in network byte order)
};
*/
_serverAddr.sin_addr.s_addr = INADDR_ANY;
if (bind(_socketFD, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) < 0) {
	std::cerr << "bind failed\n";
	close(_socketFD);
}

if (listen(_socketFD, 5) < 0) {
	std::cerr << "listen failed\n";
	close(_socketFD);
}

std::cout << "Server is listening on port 5000\n";
socklen_t clientAddrLen = sizeof(_clientAddr);
_clientSocketFD = accept(_socketFD, (struct sockaddr *)&_clientAddr, &clientAddrLen);
if (_clientSocketFD < 0) {
	std::cerr << "Could not connect with the client!\n";
}
// Prepare an HTTP response
std::string body = "Welcome to the Awesome Server!";
std::string httpResponse = "HTTP/1.1\n\n" + body;

// Send the HTTP response to the client
if (send(_clientSocketFD, httpResponse.c_str(), httpResponse.size(), 0) < 0) {
    std::cerr << "Failed to send response to the client.\n";
}
close(_clientSocketFD);
close(_socketFD);
```	
- use `http://127.0.0.1:5000/` in the browser to verify the connection

### HTTP Protocol
-Study the HTTP/1.0 and HTTP/1.1 specifications to understand how requests and responses are formatted

### Concurrency Models
- Decide on how you'll handle multiple clients—either through multi-threading or using non-blocking I/O with `select()` or `poll()`
### determine public IP address
- `curl ifconfig.me`


### CGI, select() poll(), essentiel config requirement
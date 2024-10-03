#ifndef CUSTOMSOCKET_H
# define CUSTONSOCKET_H

# include <iostream>
# include <sys/socket.h>

class CustomSocket
{
private:
	int	_fd;
public:
	CustomSocket(int domain, int type, int protocol);
	~CustomSocket();
};


#endif

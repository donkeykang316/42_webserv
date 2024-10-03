#include "CustomSocket.hpp"


CustomSocket::CustomSocket(int domain, int type, int protocol)
{
	this->_fd = socket(domain, type, protocol);
}

CustomSocket::~CustomSocket()
{
}

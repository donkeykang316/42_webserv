#ifndef HTTPPRESPONSE_H
# define HTTPPRESPONSE_H

#include "HTTP.h"
#include "HTTPRequest.hpp"
#include <sstream>
#include <cstdio>
# include <string>


class HTTPResponse
{
private:
	enum status_code_value status_code;
	HTTPRequest &_request;
	std::string status_line;
	std::string response_headers;
	std::string content;
	void	_set_content();
public:
	HTTPResponse(HTTPRequest &request);
	~HTTPResponse();
	std::string response;
	void	get();
};




#endif

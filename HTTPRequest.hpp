#ifndef HTTPREQUEST_H
# define HTTPREQUEST_H

#include "HTTP.h"
#include <vector>
// struct ServerConfig
// {
// 	std::map<std::string, std::vector<std::string>> routs;
// };
class HTTPRequest
{
private:
    enum status_code_value status_code;
    std::string buff;
    std::string method;
    std::string path;
    std::string protocol_v;
public:
    std::map<std::string, std::string> headers;
    HTTPRequest(char const * buffer);
    ~HTTPRequest();
    static const std::set<std::string> METHODS;
    enum status_code_value get_status_code();
    std::string get_method();
    std::string get_path();
    std::string get_protocol_v();
};

// const std::set<std::string> HTTPRequest::METHODS = {"GET", "POST", "DELETE"};
// HTTPRequest::METHODS->insert("GET");

#endif

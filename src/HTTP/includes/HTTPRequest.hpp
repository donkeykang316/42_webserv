#ifndef HTTPREQUEST_H
# define HTTPREQUEST_H

#include "HTTP.h"
#include "../../Configuration/includes/Dictionary.hpp"
#include "../../Configuration/includes/LocationConfig.hpp"
#include "HTTPResponse.hpp"
#include <vector>

class HTTPResponse;

class HTTPRequest
{
private:
    enum status_code_value status_code;
    std::string buff;
    std::string method;
    std::string path;
    std::map<std::string, std::string> queryParams;
    std::string protocol_v;
    std::string pathToFile;
    enum eRequestType _requestType;
    bool isHeadersSet;
    void _fillQueryParams();
public:
    bool isFulfilled;
    HTTPResponse *response;
    Dictionary &dictionary;
    LocationConfig *location;
    std::map<std::string, std::string> headers;

    HTTPRequest(Dictionary &dict);
    HTTPRequest(char const * buffer, Dictionary &dict);
    HTTPRequest &operator=(HTTPRequest &rhs);
    ~HTTPRequest();

    enum status_code_value get_status_code();
    void setStatusCode(enum status_code_value);
    std::string get_method();
    std::string get_path();
    std::map<std::string, std::string> getQueryParams();
    std::string get_protocol_v();
    enum eRequestType getRequestType();
    void setRequestType(enum eRequestType requestType);
    void urlDecode(std::string &url);
    std::string getBuffer();
    void fillRequestHeaders(char const * buffer);

    void fillRequestData(char const * buffer);
};

#endif

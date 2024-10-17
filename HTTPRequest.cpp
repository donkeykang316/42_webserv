#include "HTTPRequest.hpp"

HTTPRequest::HTTPRequest(char const *buffer)
{
    status_code = uninitialized;
    // 1. Parse first string from request
    buff.append(buffer);
    if (!buff.size())
    {
        this->status_code = bad_request;
        std::cout << "Empty request" << std::endl;
        return ;
    }
    //  set method
    this->method.append(buff.substr(0, buff.find_first_of(' ')));
    buff.erase(0, this->method.length());
    buff.erase(0, buff.find_first_not_of(" "));
    if (HTTPRequest::METHODS.find(method) == HTTPRequest::METHODS.end())
    {
        this->status_code = bad_request;
        std::cout << "NOT FOUND! " << std::endl;
    }
    std::cout << "Method is " << method << std::endl;
    // TODO:: compare method to allowed methods for location
    //  set path
    this->path.append(buff.substr(0, buff.find_first_of(' ')));
    buff.erase(0, this->path.length());
    buff.erase(0, buff.find_first_not_of(" "));
    //  set protocol

    this->protocol_v.append(buff.substr(0, buff.find_first_of('\r')));
    buff.erase(0, this->protocol_v.length() + 1);

    if (this->protocol_v.compare("HTTP/1.1") < 0)
    {
        this->status_code = bad_request;
        std::cout << " BAD PROTOCOL |" << this->protocol_v << "|" << std::endl;
        return ;

    }

    buff.erase(0, buff.find_first_not_of("\r\n"));
    // 2. Parse block of headers from request
    while (buff.find_first_of("\r\n"))
    {
        std::string header;
        header.append(buff.substr(0, buff.find_first_of('\r')));
        size_t colon_pos = header.find(':');
        if (colon_pos == header.length())
        {
            this->status_code = bad_request;
            std::cout << " Wrong header's format" << std::endl;
            return ;
        }
        buff.erase(0, header.length() + 2);

        std::string key;

        key.append(header.substr(0, colon_pos));
        header.erase(0, key.length() + 1);
        header.erase(0, header.find_first_not_of(" "));
        headers.insert(std::pair<std::string, std::string>(key, header));
    }
    buff.erase(0, buff.find_first_not_of("\r\n"));
    //  chack on header "Host"
    if (headers.find("Host") == headers.end())
    {
         this->status_code = bad_request;
            std::cout << " Wrong header's format: no info about host" << std::endl;
            return ;
    }
    // TODO:: parse body
    std::cout << "Rest is |" << buff  << "|" << std::endl;

}

HTTPRequest::~HTTPRequest()
{
}

enum status_code_value HTTPRequest::get_status_code(){
    return (status_code);
};

std::string HTTPRequest::get_method()
{
    return (method);
};

std::string HTTPRequest::get_path()
{
    return (path);
};

std::string HTTPRequest::get_protocol_v()
{
    return (protocol_v);
};
const std::string methods[] = {"GET","POST","DELETE"};
const std::set<std::string> HTTPRequest::METHODS(methods, methods + sizeof(methods)/sizeof(methods[0]));

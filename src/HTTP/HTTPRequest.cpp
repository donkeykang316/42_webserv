#include "includes/HTTPRequest.hpp"

HTTPRequest::HTTPRequest(int clFd, Dictionary &dict): dictionary(dict){
    clientFd = clFd;
    bodyToRead = 0;
    bodyLimiter.clear();
    boundary.clear();
    buff.clear();
    body.clear();
    buff = "";
    body = "";
    _requestType = UNKNOWN_REQUEST_TYPE;
    status_code = uninitialized;
    isHeadersSet = false;
    isFulfilled = false;
    response = NULL;
}
HTTPRequest &HTTPRequest::operator=(HTTPRequest &rhs)
{
    (void) rhs;
    return (*this);
}

HTTPRequest::HTTPRequest(char const *buffer, Dictionary &dict): dictionary(dict)
{
    _requestType = UNKNOWN_REQUEST_TYPE;
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
    if (!dictionary.isMethodInDictionary(method))
    {
        this->status_code = bad_request;
        std::cout << "NOT FOUND! " << method << std::endl;
    }
    // std::cout << "Method is " << method << std::endl;
    {
        if (!method.compare("GET"))
            _requestType = GET_FILE;
        else if (!method.compare("POST"))
            _requestType = POST_DATA;
        else if (!method.compare("DELETE"))
            _requestType = DELETE_DATA;
    }
    //  set path
    this->path.append(buff.substr(0, buff.find_first_of(' ')));
    buff.erase(0, this->path.length());
    buff.erase(0, buff.find_first_not_of(" "));
    size_t queryParamsStartPos = this->path.find('?');
    if (queryParamsStartPos != std::string::npos)
    {
        std::cout <<"QUERRY!!!!!!!" << std::endl;
        std::string queryParamsStr = this->path.substr(queryParamsStartPos + 1);
        this->path.erase(this->path.begin() + queryParamsStartPos, this->path.end());
        // set parameters
        while (!queryParamsStr.empty())
        {
            size_t equalSignPos = queryParamsStr.find_first_of('=');
            size_t ampersandPos = queryParamsStr.find_first_of('&');
            if ((equalSignPos == std::string::npos || equalSignPos == queryParamsStr.size() - 1)
                || (ampersandPos != std::string::npos && ampersandPos < equalSignPos))
            {
                //TODO:: check err code for query params
                this->status_code = bad_request;
                break;
            }
            // if ()
            std::string first = queryParamsStr.substr(0, equalSignPos);
            size_t endOfSecondPos = queryParamsStr.size();
            if (ampersandPos != std::string::npos)
                endOfSecondPos = ampersandPos;
            std::string second = queryParamsStr.substr(equalSignPos + 1, endOfSecondPos);
            queryParamsStr.erase(queryParamsStr.begin(), queryParamsStr.begin() + endOfSecondPos);
            if (!queryParamsStr.empty() && queryParamsStr[0] == '&')
                queryParamsStr.erase(queryParamsStr.begin(), queryParamsStr.begin() + 1);
            queryParams[first] = second;
        }
    }
    //  set protocol
    std::cout << "REST OF BUFF "<< buff.substr(0, buff.find_first_of('\r')) << std::endl;
    this->protocol_v.append(buff.substr(0, buff.find_first_of('\r')));
    buff.erase(0, this->protocol_v.length() + 1);

    if (!this->protocol_v.compare("HTTP/1.1"))
    {
        this->status_code = bad_request;
        std::cout << " BAD PROTOCOL |" << this->protocol_v << "|" << std::endl;
        return ;

    }
	std::cout << "PROTOCOL " << protocol_v << " " << get_protocol_v() << std::endl;

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
        std::cout << "Key: " << key << std::endl;
        headers.insert(std::pair<std::string, std::string>(key, header));
        if (!key.compare("Content-Length"))
        {
            std::stringstream sstream(header);
            size_t result;
            sstream >> result;
            bodyToRead = result;
            std::cout << ">>>>>>>>>>>>>>>>>>>Content-Length: " << bodyToRead<< std::endl;
        }
        if (!key.compare("Content-Type"))
        {
            std::string bounadyTitle = "boundary=";
            size_t boudatyStart = header.find(bounadyTitle);
            if (boudatyStart < header.size())
            {
                bodyLimiter.append(header.substr(boudatyStart + bounadyTitle.size()));
                bodyLimiter.append("--\r\n\r\n");
            }
            std::cout << ">>>>>>>>>>>>>>>>>>>Content-Type: " << bodyLimiter<< std::endl;
        }
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
    // std::cout << "Rest is |" << buff  << "|" << std::endl;

}

HTTPRequest::~HTTPRequest()
{
    delete response;
}

enum status_code_value HTTPRequest::get_status_code(){
    return (status_code);
};

void HTTPRequest::setStatusCode(enum status_code_value code){
    status_code = code;
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

enum eRequestType HTTPRequest::getRequestType(){
    return (_requestType);
};

void HTTPRequest::setRequestType(enum eRequestType requestType)
{
    _requestType = requestType;
};

void HTTPRequest::urlDecode(std::string &encodedString)
{
    std::string hexChars = "0123456789ABCDEFabcdef";
    std::string decodedChar ="";
    size_t posToDecode = encodedString.find_first_of('%', 0);
    while (
        (posToDecode !=std::string::npos)
        &&(posToDecode + 2 < encodedString.size())
        )
    {
        decodedChar.clear();
        if (
            (hexChars.find(encodedString[posToDecode + 1]) !=std::string::npos) && (hexChars.find(encodedString[posToDecode + 2]) != std::string::npos)
            )
        {
           int hexParts[2];
           hexParts[0] = hexChars.find(encodedString[posToDecode + 1]);
           hexParts[1] = hexChars.find(encodedString[posToDecode + 2]);
           for (int i = 0; i < 2; i++)
           {
               if (hexParts[i] / 16)
                hexParts[i] -= 6;
           }
           decodedChar.insert(decodedChar.begin(), (unsigned char)(hexParts[0] * 16 + hexParts[1]));
           std::cout  << decodedChar << std::endl;
           encodedString.replace(encodedString.begin() + posToDecode, encodedString.begin() + posToDecode + 3, decodedChar);
           posToDecode++;
        }
        else
            posToDecode += 3;
        posToDecode = encodedString.find_first_of('%', posToDecode);
    }
}

std::string HTTPRequest::getBuffer()
{
    return (this->buff);
}

void HTTPRequest::fillRequestHeaders(char const * buffer)
{
    buff.append(buffer);
    // 2. Parse block of headers from request
    while (buff.find_first_of("\r\n"))
    {
        if (buff.find_first_of('\r') == std::string::npos)
            return ;
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
        headers.insert(std::pair<std::string, std::string>(key, header));
        if (!key.compare("Content-Length"))
        {
            std::stringstream sstream(header);
            size_t result;
            sstream >> result;
            bodyToRead = result;
            std::cout << ">>>>>>>>>>>>>>>>>>>Content-Length: " << bodyToRead<< std::endl;
        }
        if (!key.compare("Content-Type"))
        {
            std::string bounadyTitle = "boundary=";
            size_t boudatyStart = header.find(bounadyTitle);
            if (boudatyStart < header.size())
            {
                boundary = header.substr(boudatyStart + bounadyTitle.size());
                bodyLimiter.append(boundary);
                bodyLimiter.append("--\r\n\r\n");
            }
            std::cout << ">>>>>>>>>>>>>>>>>>>Content-Type: " << bodyLimiter<< std::endl;
        }
    }
    buff.erase(0, buff.find_first_not_of("\r\n"));
    isHeadersSet = true;
    // this->isFulfilled = true;
    //  chack on header "Host"
    if (headers.find("Host") == headers.end())
    {
         this->status_code = bad_request;
            std::cout << " Wrong header's format: no info about host" << std::endl;
            return ;
    }

}

void HTTPRequest::_fillQueryParams()
{
    size_t queryParamsStartPos = this->path.find('?');
    if (queryParamsStartPos != std::string::npos)
    {
        std::cout << "QUERRY!!!!!" << std::endl;
        std::string queryParamsStr = this->path.substr(queryParamsStartPos + 1);
        this->path.erase(this->path.begin() + queryParamsStartPos, this->path.end());
        // set parameters
        while (!queryParamsStr.empty())
        {
            size_t equalSignPos = queryParamsStr.find_first_of('=');
            size_t ampersandPos = queryParamsStr.find_first_of('&');
            if ((equalSignPos == std::string::npos || equalSignPos == queryParamsStr.size() - 1)
                || (ampersandPos != std::string::npos && ampersandPos < equalSignPos))
            {
                //TODO:: check err code for query params
                this->status_code = bad_request;
                break;
            }
            std::string first = queryParamsStr.substr(0, equalSignPos);
            size_t endOfSecondPos = queryParamsStr.size();
            if (ampersandPos != std::string::npos)
                endOfSecondPos = ampersandPos;
            std::string second = queryParamsStr.substr(equalSignPos + 1, endOfSecondPos);
            queryParamsStr.erase(queryParamsStr.begin(), queryParamsStr.begin() + endOfSecondPos);
            if (!queryParamsStr.empty() && queryParamsStr[0] == '&')
                queryParamsStr.erase(queryParamsStr.begin(), queryParamsStr.begin() + 1);
            queryParams[first] = second;
        }
    }
}

std::map<std::string, std::string> HTTPRequest::getQueryParams()
{
    return (this->queryParams);
}


void HTTPRequest::fillRequestData(char const * buffer)
{
    // std::cout << "<< FILL REQUEST DATA " << std::endl;
    // std::cout << buffer << std::endl;
    // std::cout << ">> FILL REQUEST DATA " << std::endl;
  
    // std::string tmp;
    // tmp.append(buffer);
    // std::size_t key_index = tmp.find("\r\n\r\n");
    // bool endFile = (key_index != tmp.npos);
    // std::cout << "Do we have \r\n\r\n :" << (endFile == std::string::npos) << " " << (endFile != std::string::npos) << std::endl;
    // if (key_index < tmp.size() && isHeadersSet)
    // {
    //     std::cout << "SET fulfield" << tmp.find("\r\n\r\n") << " " << std::string::npos << std::endl;
    //     isFulfilled = true;
    // }
    // tmp.clear();
    // Header has been read, body data should be sent in response
    // if (response && !(*buffer) && buff.size() == 0)
    //     close(response->tubes[1]);
    if (isHeadersSet)
    {
        // TODO::if needed to read body
        // if (buff.size())
            // buff.resize(buff.size()-1);
        buff.append(buffer);
        if (buff.size() > bodyLimiter.size())
        {
            lastBuff.clear();
        }
        lastBuff.append(buff);
        // if (bodyToRead > 0)
        // {
        //     bodyToRead -= buff.size();
        //     std::cout << "~~~~~~~~~BODY TO READ: " << bodyToRead << std::endl;
        // }
        if (bodyToRead <= 0 || (bodyToRead > 0 && lastBuff.find(bodyLimiter) < buff.size()))
        {
            isFulfilled = true;
            std::cout << "SET fulfield" << std::endl;
        }

        // if (buff.size())
        response->setRequestData(buff.c_str());
        // response->setRequestData(buffer);
        // buff.append(buffer);
        // if (isFulfilled)
        // {
        // response->setRequestData(buff.c_str());
        buff.clear();
        // }
        return ;
    }
    // Waiting for the next chunk of header data
    else if (_requestType != UNKNOWN_REQUEST_TYPE && !isHeadersSet)
        return ;
    if (_requestType != UNKNOWN_REQUEST_TYPE && !isHeadersSet)
    {
        fillRequestHeaders(buffer);
    }


    // if request reads the buffer for the first time
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
    if (!dictionary.isMethodInDictionary(method))
    {
        this->status_code = bad_request;
        std::cout << "NOT FOUND! - " << method << std::endl;
    }
    if (!method.compare("GET"))
        _requestType = GET_FILE;
    else if (!method.compare("POST"))
        _requestType = POST_DATA;
    else if (!method.compare("DELETE"))
        _requestType = DELETE_DATA;
    //  set path
    this->path.append(buff.substr(0, buff.find_first_of(' ')));
    buff.erase(0, this->path.length());
    buff.erase(0, buff.find_first_not_of(" "));
     //  set protocol
    // std::cout << "REST OF BUFF "<< buff.substr(0, buff.find_first_of('\r')) << std::endl;
    this->protocol_v.append(buff.substr(0, buff.find_first_of('\r')));
    buff.erase(0, this->protocol_v.length() + 1);

    if (this->protocol_v.compare("HTTP/1.1"))
    {
        this->status_code = bad_request;
        std::cout << " BAD PROTOCOL |" << this->protocol_v << "|" << std::endl;
        return ;

    }
	// std::cout << "PROTOCOL " << protocol_v << " " << get_protocol_v() << std::endl;

    buff.erase(0, buff.find_first_not_of("\r\n"));
    _fillQueryParams();
    fillRequestHeaders("");
    // std::cout << "<><>BUFF: " << buff << std::endl;
    // std::cout << "><><>BUFF: " << std::endl;
}

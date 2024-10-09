void setupEnv(std::map<std::string, std::string>& envVars,
                    const std::string& method,
                    const std::string& queryString,
                    const std::string& contentLength,
                    const std::string& contentType,
                    const std::string& FileName,
                    const std::string& serverProtocol,
                    const std::string& remoteAddr,
                    const std::string& remotePort) {
    envVars["REQUEST_METHOD"] = method; // GET, POST, HEAD, PUT, DELETE example: REQUEST_METHOD=GET
    envVars["QUERY_STRING"] = queryString; // example: QUERY_STRING=name=John&age=30
    envVars["CONTENT_LENGTH"] = contentLength; // example: CONTENT_LENGTH=123
    envVars["CONTENT_TYPE"] = contentType; // example: CONTENT_TYPE=text/html
    envVars["FILE_NAME"] = FileName; // example: SCRIPT_FILENAME=/usr/local/apache/cgi-bin/script.cgi
    envVars["SERVER_PROTOCOL"] = serverProtocol; // example: SERVER_PROTOCOL=HTTP/1.1 or SERVER_PROTOCOL=CGI/1.1
    envVars["REMOTE_ADDR"] = remoteAddr; // example: REMOTE_ADDR=127.0.0.1
    envVars["REMOTE_PORT"] = remotePort; // example: REMOTE_PORT=5000
}
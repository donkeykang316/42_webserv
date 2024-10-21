#include "includes/CGI.hpp"

cgi::cgi(HTTPRequest &request, std::string filePath) : _request(request), _filePath(filePath) {}

cgi::~cgi() {}

void cgi::cgiHandler() {
    std::cout << "CGI HANDLER STARTED" << std::endl;

    // Determine status code
    status_code_value status_code = _request.get_status_code();
    if (status_code == uninitialized) {
        status_code = ok;
    }

    // Construct status line
    std::ostringstream stat_code_str;
    stat_code_str << status_code;
    status_line = _request.get_protocol_v() + " " + stat_code_str.str() + " " + 
                  ((status_code == ok) ? "OK" : "Bad Request") + "\r\n";

    // Construct response headers
    response_headers = "Connection: close\r\n";

    // Content-Type based on file extension
    std::string fileExtension = "";
    std::string contentType = "text/html";
    envInit();

    std::string filePathTest = _filePath.substr(1, _filePath.size() - 1);
	std::string cmd = "python3";

    if (_request.get_method() == "POST") {
        executeCGI(filePathTest, cmd);
    } else {
        // Handle other methods if necessary
    }

    // Construct the final response
    _cgiResponse = status_line + response_headers;

    // Append CGI body if present
    if (!_cgiBody.empty()) {
        std::ostringstream content_length;
        content_length << "Content-Length: " << _cgiBody.size() << "\r\n";
        _cgiResponse += content_length.str();
        _cgiResponse += "\r\n" + _cgiBody;
    } else {
        _cgiResponse += "Content-Length: 0\r\n\r\n";
    }

    std::cout << "CGI Handler Completed. Response:\n" << _cgiResponse << std::endl;
}

void cgi::envInit() {
	_env.clear();
    _env.push_back("AUTH_TYPE=Basic");
	_env.push_back("REDIRECT_STATUS=200");
	_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    _env.push_back("CONTENT_TYPE=" + _request.headers["Content-Type"]);
	_env.push_back("SCRIPT_NAME=" + _request.get_path());
	_env.push_back("SCRIPT_FILENAME=" + _request.get_path());
	_env.push_back("REQUEST_URI=" + _request.get_path());
	_env.push_back("REQUEST_METHOD=" + _request.get_method());
	_env.push_back("SERVER_PROTOCOL=" + _request.get_protocol_v());
	_env.push_back("HTTP_ACCEPT=" + _request.headers["Accept"]);
	_env.push_back("HTTP_ACCEPT_CHARSET=" + _request.headers["Accept-Charset"]);
	_env.push_back("HTTP_ACCEPT_LANGUAGE=" + _request.headers["Accept-Language"]);
	_env.push_back("HTTP_CONNECTION=" + _request.headers["Connection"]);
	_env.push_back("HTTP_HOST=" + _request.headers["Host"]);
	_env.push_back("HTTP_REFERER=" + _request.headers["Referer"]);
	_env.push_back("HTTP_USER_AGENT=" + _request.headers["User-Agent"]);
	_env.push_back("CONTENT_LENGTH=" + _request.headers["Content-Length"]);
    _env.push_back("filename=random");
	if (_request.headers.find("Referer") != _request.headers.end())
    	_env.push_back("HTTP_REFERER=" + _request.headers["Referer"]);
}

void cgi::executeCGI(const std::string& filePath, const std::string& cmd) {
    int inPipe[2];
    int outPipe[2];

    if (pipe(inPipe) == -1 || pipe(outPipe) == -1) {
        std::cerr << "Failed to create pipes: " << strerror(errno) << std::endl;
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Failed to fork: " << strerror(errno) << std::endl;
        return;
    }

    if (pid == 0) {
        // Child process
        close(inPipe[1]);  // Close unused write end
        close(outPipe[0]); // Close unused read end

        // Redirect stdin and stdout
        if (dup2(inPipe[0], STDIN_FILENO) == -1) {
            std::cerr << "dup2 failed for stdin: " << strerror(errno) << std::endl;
            std::exit(1);
        }
        if (dup2(outPipe[1], STDOUT_FILENO) == -1) {
            std::cerr << "dup2 failed for stdout: " << strerror(errno) << std::endl;
            std::exit(1);
        }

        // Close original pipe descriptors after duplication
        close(inPipe[0]);
        close(outPipe[1]);

        // Absolute path to the Python interpreter
        std::string cmdPath = "/usr/bin/python3"; // Verify the correct path on your system

        // Prepare arguments
        char *const argv[] = { const_cast<char*>(cmd.c_str()), const_cast<char*>(filePath.c_str()), NULL };

        // Prepare environment variables
        char** envp = new char*[_env.size() + 1];
        for (size_t i = 0; i < _env.size(); ++i) {
            envp[i] = new char[_env[i].size() + 1];
            std::strcpy(envp[i], _env[i].c_str());
        }
        envp[_env.size()] = NULL;

        std::cerr << std::endl;
	/*std::cerr << "_chENV:\n";
	for (int i = 0; envp[i] != NULL; ++i) {
        std::cerr << "" << envp[i] << std::endl;
    }*/

        // Execute the CGI script
        if (execve(cmdPath.c_str(), argv, envp) == -1) {
            std::cerr << "Execution failed: " << strerror(errno) << std::endl;
            std::exit(1);
        }
    } else {
        // Parent process
        close(inPipe[0]);  // Close unused read end
        close(outPipe[1]); // Close unused write end

        // If POST, write the request body to the child process's stdin
        if (_request.get_method() == "POST") {
            const std::string &body = _request.get_body();
            ssize_t totalWritten = 0;
            ssize_t bodySize = body.size();

            std::cout << "Writing " << bodySize << " bytes to CGI stdin." << std::endl;

            while (totalWritten < bodySize) {
                ssize_t written = write(inPipe[1], body.c_str() + totalWritten, bodySize - totalWritten);
                if (written == -1) {
                    std::cerr << "Failed to write to CGI stdin: " << strerror(errno) << std::endl;
                    break;
                }
                totalWritten += written;
            }

            std::cout << "Finished writing to CGI stdin." << std::endl;
        }

        close(inPipe[1]); // Signal EOF to the child

        // Read CGI output
        char buffer[4096];
        ssize_t bytesRead;
        while ((bytesRead = read(outPipe[0], buffer, sizeof(buffer))) > 0) {
            _cgiBody.append(buffer, bytesRead);
        }
        close(outPipe[0]);

        // Wait for the child process to finish
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            std::cerr << "waitpid failed: " << strerror(errno) << std::endl;
            return;
        }
        if (WIFEXITED(status)) {
            std::cout << "Child exited with status " << WEXITSTATUS(status) << std::endl;
        } else if (WIFSIGNALED(status)) {
            std::cout << "Child killed by signal " << WTERMSIG(status) << std::endl;
        }

        // Set the CGI response
        _cgiResponse = _cgiBody;
    }
}
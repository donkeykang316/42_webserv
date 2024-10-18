#include "includes/CGI.hpp"

cgi::cgi(HTTPRequest &request, std::string filePath) : _request(request) {
    status_code = _request.get_status_code();
	if (!status_code)
		status_code = ok;
	std::cout << "1 RESPONSE " << std::endl;
	status_line.append(_request.get_protocol_v());
	status_line.append(" ");
	std::ostringstream stat_code_str;
	stat_code_str << status_code;
	status_line.append(stat_code_str.str());
	status_line.append(" ");
	status_line.append(getStatusCodeMsg(status_code));
	status_line.append("\r\n");
	std::cout << "2 RESPONSE " << std::endl;
	if (_request.headers.find("Connection") == _request.headers.end())
		response_headers.append("Connection: keep-alive\r\n");
	else
	{
		response_headers.append("Connection: ");
		response_headers.append(_request.headers["Connection"]);
		response_headers.append("\r\n");
	}
	std::cout << "3 RESPONSE " << std::endl;
	response_headers.append("Content-Type: ");

	std::string fileExtention = getUriExtention(filePath);

	//  fileExtention = getUriExtention(filePath);
	std::cout << "fileExtention " << fileExtention << " filePath " << filePath.empty() << " getContentTypeFromDictionary " <<_request.dictionary.getContentTypeFromDictionary(fileExtention) << " " <<_request.dictionary.getContentTypeFromDictionary("py") << std::endl;
	response_headers.append(_request.dictionary.getContentTypeFromDictionary(fileExtention));
	response_headers.append("\r\n");
	std::cout << "4 RESPONSE " << std::endl;
	_set_content(filePath);
	std::cout << "5 RESPONSE " << std::endl;
	_cgiResponse.append(status_line);
	_cgiResponse.append(response_headers);
	_cgiResponse.append("Content-Length: ");
	std::ostringstream cont_l;
	cont_l << content.size();
	_cgiResponse.append(cont_l.str());
	_cgiResponse.append("\r\n");
	_cgiResponse.append("\r\n");
	std::cout << "6 RESPONSE " << std::endl;
	_cgiResponse.append(content);

	/*if (request.get_method().compare("POST"))
		executeCGI();*/
}

cgi::~cgi() {}

void cgi::_set_content(std::string filePath)
{
	if (filePath.empty())
	{
		content.clear();
		content.append(getDefaultErrorPageContent(_request.get_status_code()));
		return ;
	}
	FILE *fileToRead = std::fopen(filePath.c_str(), "r");
	if (fileToRead == NULL)
	{
		std::cerr << "file open error\n";
		content.clear();
		content.append(getDefaultErrorPageContent(_request.get_status_code()));
		// std::fclose(fileToRead);
		return ;

	}
	int c;
	while ((c = std::fgetc(fileToRead)) != EOF)
	{
		char ch = c;
		std::stringstream ss;
		ss << ch;
		content += ss.str();
	}

	// Close the file
	std::fclose(fileToRead);
	std::cout << "\n\n"; // Add spacing between files
}

std::string cgi::getDefaultErrorPageContent(enum status_code_value statusCode)
{
	std::string content;
	content.append("<!DOCTYPE html>");
	content.append("<head>");
	content.append("</head>");
	content.append("<div id=\"main\" style=\"display: table; width: 100%; height: 100vh; text-align: center; \">\r\n");
    content.append("	<div class=\"fof\" style=\"display: table-cell; vertical-align: middle; \"\r\n>");
	content.append("   		<h1 style=\"font-size: 50px; display: inline-block; padding-right: 12px; animation: type .5s alternate infinite; \">Error ");
	std::ostringstream stat_code_str;
		stat_code_str << statusCode;
	content.append(stat_code_str.str());
	content.append("</h1>\r\n");
    content.append("	</div>\r\n");
	content.append("</div>");
	return (content);
}

void cgi::envInit() {
	_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    _env.push_back("SERVER_PROTOCOL=HTTP/1.1");
    _env.push_back("REQUEST_METHOD=GET");
    _env.push_back("SCRIPT_NAME=/cgi-bin/test.cgi");
    _env.push_back("QUERY_STRING=");
    _env.push_back("CONTENT_TYPE=");
    _env.push_back("CONTENT_LENGTH=");
    _env.push_back("SERVER_SOFTWARE=MyServer/1.0");
    _env.push_back("SERVER_NAME=localhost");
    _env.push_back("SERVER_PORT=80");
    _env.push_back("REMOTE_ADDR=127.0.0.1");
    _env.push_back("REMOTE_HOST=localhost");
    _env.push_back("AUTH_TYPE=");
    _env.push_back("REMOTE_USER=");
    _env.push_back("REMOTE_IDENT=");
    _env.push_back("HTTP_USER_AGENT=Mozilla/5.0");
    _env.push_back("HTTP_ACCEPT=text/html");
}

void cgi::resetHeaders()
{
	_headers.clear();

	_headers["Accept-Charsets"] = "";
	_headers["Accept-Language"] = "";
	_headers["Allow"] = "";
	_headers["Auth-Scheme"] = "";
	_headers["Authorization"] = "";
	_headers["Content-Language"] = "";
	_headers["Content-Length"] = "";
	_headers["Content-Location"] = "";
	_headers["Content-Type"] = "";
	_headers["Date"] = "";
	_headers["Host"] = "";
	_headers["Last-Modified"] = "";
	_headers["Location"] = "";
	_headers["Referer"] = "";
	_headers["Retry-After"] = "";
	_headers["Server"] = "";
	_headers["Transfer-Encoding"] = "";
	_headers["User-Agent"] = "";
	_headers["Www-Authenticate"] = "";
	_headers["Connection"] = "Keep-Alive";
}

void cgi::executeCGI(std::string &filePath, std::string &cmd) {
	int inPipe[2];
    int outPipe[2];

	//resetHeaders();
	envInit();
	if (pipe(inPipe) == -1 || pipe(outPipe) == -1) {
		std::cerr << "Failed to create pipes.\n";
		return;
	}

	pid_t pid = fork();
	if (pid == -1) {
		std::cerr << "Failed to fork.\n";
		return;
	}

	if (pid == 0) {
		// Child process
		close(inPipe[1]);
		close(outPipe[0]);

		// Redirect stdin and stdout
		dup2(inPipe[0], STDIN_FILENO);
		dup2(outPipe[1], STDOUT_FILENO);

		std::string cmdPath = "/bin/" + cmd;
    	char *const argv[] = { const_cast<char*>(cmd.c_str()), const_cast<char*>(filePath.c_str()), NULL };
		std::cerr << "filePath: " << filePath << std::endl;

		std::vector<char*> envp;
    	for (size_t i = 0; i < _env.size(); ++i) {
        	envp.push_back(const_cast<char*>(_env[i].c_str()));
    	}
    	envp.push_back(NULL);

		// Execute the CGI script
		if (execve(cmdPath.c_str(), argv, &envp[0]) == -1) {
			std::cerr << "Execution failed!\n";
			std::exit(1);
		}
		std::exit(1);
	}

	int status;
    if (waitpid(pid, &status, 0) == -1) {
        std::cerr << "waitpid failed: " << strerror(errno) << std::endl;
            return ;
    }
    if (WIFEXITED(status)) {
        std::cout << "Child exited with status " << WEXITSTATUS(status) << std::endl;
    }
    else if (WIFSIGNALED(status)) {
        std::cout << "Child killed by signal " << WTERMSIG(status) << std::endl;
    }

	close(outPipe[1]);

	char buffer[1024];
    ssize_t bytesRead;
    while ((bytesRead = read(outPipe[0], buffer, sizeof(buffer))) > 0) {
            _cgiResponse.append(buffer, bytesRead);
    }
	close(outPipe[0]);
    
}
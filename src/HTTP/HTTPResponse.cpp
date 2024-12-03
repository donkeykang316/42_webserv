#include "includes/HTTPResponse.hpp"

HTTPResponse::HTTPResponse(int clFd, HTTPRequest &request, std::string filePath) : _request(request)
{
	clientFd = clFd;
	if (request.getRequestType()== POST_DATA || request.getRequestType() == DELETE_DATA)
		isFulfilled = false;
	else
		isFulfilled = true;

	response.clear();
		std::cout << "HTTPResponse::HTTPResponse" << std::endl;
	if (_request.location->isCgi)
	{
		std::cout << "Problem is here" << std::endl;
		runCGI(_request.location, &_request);
		return ;
	}
	// if ()
	status_code = _request.get_status_code();
	if (!status_code)
		status_code = ok;
	std::cout << "PROTOCOL " << _request.get_protocol_v() << std::endl;
	status_line.append(_request.get_protocol_v());
	status_line.append(" ");
	std::ostringstream stat_code_str;
	stat_code_str << status_code;
	status_line.append(stat_code_str.str());
	status_line.append(" ");
	status_line.append(getStatusCodeMsg(status_code));
	status_line.append("\r\n");
	std::cout << "status_line "<< status_line << std::endl;
	response.append(status_line);

	if (_request.getRequestType() == REDIRECT_REQUEST)
	{
		response_headers.append("Location: ");
		response_headers.append(filePath);
		response_headers.append("\r\n");
		response_headers.append("Content-Type: text/plain; charset=utf-8\r\n");
		response.append(response_headers);
		response.append("\r\n");
		response.append("\r\n");
		std::cout << "RESPONSE REDIRECT_REQUEST " << response << "|" << std::endl;
		isFulfilled = true;
		return ;
	}

	if (_request.headers.find("Connection") == _request.headers.end())
		response_headers.append("Connection: keep-alive\r\n");
	else
	{
		response_headers.append("Connection: ");
		response_headers.append(_request.headers["Connection"]);
		response_headers.append("\r\n");
	}
	response_headers.append("Content-Type: ");

	std::string fileExtention = getUriExtention(filePath);
	if (request.getRequestType() == GET_DIR_LIST)
		fileExtention.replace(fileExtention.begin(), fileExtention.end(), "html");
	response_headers.append(_request.dictionary.getContentTypeFromDictionary(fileExtention));
	response_headers.append("\r\n");
	if (request.getRequestType() == GET_DIR_LIST)
		content.append(getDirectoryListing(filePath, request.get_path()));
	else
		_set_content(filePath);
	response.append(response_headers);
	response.append("Content-Length: ");
	std::ostringstream cont_l;
	cont_l << content.size();
	response.append(cont_l.str());
	response.append("\r\n");
	response.append("\r\n");
	response.append(content);
	this->isFulfilled = true;
}

HTTPResponse::~HTTPResponse()
{
}

void HTTPResponse::get()
{
}

void HTTPResponse::_set_content(std::string filePath)
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

std::string HTTPResponse::getDefaultErrorPageContent(enum status_code_value statusCode)
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

std::string HTTPResponse::getDirectoryListing(std::string filePath, std::string location)
{
	std::string content;
	content.append("<!DOCTYPE html>");
	content.append("<head>");
	content.append("</head>");
	DIR *dir;
	struct dirent *ent;
	std::string relativePath = ".";
	relativePath.append(filePath);
	if ((dir = opendir (relativePath.c_str())) != NULL) {
  	/* print all the files and directories within directory */
		content.append("<h1> List of directory files: </h1>\r\n");
		content.append("<div style=\"display: table; width: 100%; text-align: left; margin-left:10px \">\r\n");
		while ((ent = readdir(dir)) != NULL)
		{
		    content.append("	<div class=\"fof\" style=\"display: table-row; height: 25px; vertical-align: middle; \"\r\n>");
			content.append("<a href=\"");
			content.append(location);
			if (location.size() > 1 && location.find_last_of('/') != location.size() - 1)
				content.append("/");
			content.append(ent->d_name);
			content.append("\">\r\n");
			content.append(ent->d_name);
			content.append(" ");
			content.append("</a>\r\n");
			content.append("</div>\r\n");

		}
		content.append("</div>\r\n");
		closedir (dir);

	}
	else {
  	/* could not open directory */
  		perror ("");
	}
	return (content);
}

void HTTPResponse::urlEncode(std::string &string)
{
    std::string allowedChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-_.~";
    std::string hexChars = allowedChars.substr(0, 16);
    char charToEncode;
    size_t posToEncode = string.find_first_not_of(allowedChars, 0);
    while (
        (posToEncode !=std::string::npos)
        &&(posToEncode< string.size())
        )
    {
        charToEncode=string[posToEncode];
        string[posToEncode]='%';
        string.insert(string.begin() + posToEncode + 1,hexChars[charToEncode&0xf]);
        string.insert(string.begin() + posToEncode + 1,hexChars[charToEncode>>4]);
        posToEncode+=3;
        posToEncode = string.find_first_not_of(allowedChars, posToEncode);
    }
}

void HTTPResponse::sendResponse()
{
	std::cout << MAGENTA << "SEND DATA " << RESET << std::endl;
	eRequestType r_type = _request.getRequestType();

	if (!_request.location || !(_request.location->isCgi))
		return ;
	if (r_type == POST_DATA && !_request.isFulfilled)
		return ;
	std::cout << MAGENTA << "SEND DATA CGI " << _request.location->isCgi << RESET << std::endl;

	std::string s;
	char ch;
	while (read(cgiResponseFds[0], &ch, 1) > 0)
	{
		s.push_back(ch);
		if (ch == '\n')
			break;
	}
	std::cout << "First string: " << s << std::endl;
	if (s.find("Status: ") == 0)
	{
		s.erase(s.begin(), s.begin() + 7);
		response.append(_request.get_protocol_v());
		response.append(s);
	}
	s.clear();

	while (read(cgiResponseFds[0], &ch, 1) > 0)
	{
			response.push_back(ch);
	}
	std::cout << "Rest response: " << response << std::endl;

	isFulfilled = true;
	close(cgiResponseFds[0]);
	std::cout << MAGENTA << "\n\n--2.4-- PARENT CLOSE RESPONSE tubes (sendResponse)\n\n" << RESET << std::endl;

}

void HTTPResponse::setRequestData(const char *buff, ssize_t len)
{
	if (_request.location && _request.location->isCgi)
	{
		std::string buffer = "";
		buffer.append(buff, len);

		if (buffer.size())
		{
			if (read(tubes[0], NULL, 0) == 0)
			{
				write(tubes[1], buffer.c_str(), buffer.size());
			}
			else
				std::cout << RED << "Is not readable " << read(tubes[0], NULL, 0) << RESET << std::endl;
		}
		if (_request.isFulfilled)
		{
			close(tubes[1]);
			close(tubes[0]);
			std::cout << MAGENTA << "\n\n--1.4-- PARENT CLOSE PIPE tubes\n\n" << RESET << std::endl;

		}
	}
}

void HTTPResponse::getEnvVariables(char ***envp)
{

	std::map<std::string,std::string> env;
	// env["INI_PERDIR"] = "20M";
	if (_request.headers.find("Content-Length") != _request.headers.end())
		env["CONTENT_LENGTH"] = _request.headers["Content-Length"];
	if (_request.headers.find("Content-Type") != _request.headers.end())
		env["CONTENT_TYPE"] = _request.headers["Content-Type"];
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	env["SERVER_PROTOCOL"] = _request.get_protocol_v();
	//TODO:take from cgi params and request query params
	env["PATH_INFO"] = "index.php";
	env["QUERY_STRING"] = "";
	std::map<std::string, std::string>queryParams = _request.getQueryParams();
	for (std::map<std::string, std::string>::iterator queryParamIt = queryParams.begin(); queryParamIt != queryParams.end(); queryParamIt++)
	{
		if (queryParamIt != queryParams.begin())
			env["QUERY_STRING"].append("&");
		env["QUERY_STRING"].append(queryParamIt->first);
		env["QUERY_STRING"].append("=");
		env["QUERY_STRING"].append(queryParamIt->second);
	}
	env["REQUEST_METHOD"] = _request.get_method();
	env["SCRIPT_NAME"] = "./cgi-bin/index.php"; //_request.get_path();
	env["SCRIPT_FILENAME"] = "./cgi-bin/index.php"; //_request.get_path();
	env["REDIRECT_STATUS"] = "CGI";
	*envp = new char *[env.size() + 1];
	size_t idx = 0;
	for (std::map<std::string, std::string>::iterator envIt = env.begin(); envIt != env.end(); envIt++)
	{
		std::string element = envIt->first + "=" + envIt->second;
		(*envp)[idx] = new char [element.size() + 1];
		for (size_t i = 0 ; i < element.size(); i++)
			(*envp)[idx][i] = element[i];
		(*envp)[idx][element.size()] = '\0';
		idx++;
	}
	(*envp)[idx] = NULL;
}

void HTTPResponse::clearEnvVariables(char ***envp)
{
	char **env = *envp;
	size_t idx = 0;
	while (env[idx])
	{
		delete [] (env[idx]);
		idx++;
	}
	delete [] env;
}

void HTTPResponse::runCGI(LocationConfig *location, HTTPRequest *request)
{
	std::cout << GREEN << " ~~~ " << location->cgiInclude << " " << location->cgiParams.begin()->first << " " << location->cgiParams.begin()->second<< RESET << std::endl;
	std::cout << GREEN << " CGI root " << location->root << RESET << std::endl;
	std::string filePath = request->get_path();
	std::string ext = location->getCgiExtentionFromUri(filePath);
	std::string supportedExt = request->dictionary.getSupportedCGIExtension(ext);
	std::cout << "First get  filePath: " << filePath <<" ext: " << ext << " supportedExt " << supportedExt << std::endl;
	if ((ext.empty() || ext.empty()) && !location->cgiIndex.empty() )
	{
		filePath.append("/");
		filePath.append(location->cgiIndex);
		ext = location->getCgiExtentionFromUri(location->cgiIndex);
		supportedExt = request->dictionary.getSupportedCGIExtension(ext);
		std::cout << "Second get filePath: " << filePath <<" ext: " << ext << " supportedExt " << supportedExt << std::endl;
	}
	if (supportedExt.empty() && request->get_method().compare("DELETE"))
	{
		std::cout << "Incorrect data in config for cgi" << std::endl;
		return ;
	}

	pipe(tubes);
	std::cout << CYAN << "\n\n--1.1-- PIPE tubes " << tubes[0] << " " << tubes[1] << "\n\n" << RESET << std::endl;
	pipe(cgiResponseFds);
	std::cout << CYAN  << "---cgiResponseFds(pipe) is " << cgiResponseFds[0] << " "<< cgiResponseFds[1] << "\n" << RESET << std::endl;

	if (!request->get_method().compare("DELETE"))
	{
		supportedExt.clear();
		supportedExt = "php";
	}
	scriptExec = request->dictionary.getSupportedCGIExecutor(supportedExt);
	std::string scriptExecFlag = request->dictionary.getSupportedCGIExecutorFlag(supportedExt);

	pid_t cgi_pid;
	char * paramsList[4];

	size_t idx = 0;
	std::cout << GREEN << "~~~~ Executor: " << request->dictionary.getSupportedCGIExecutor(supportedExt) << " flag: " << request->dictionary.getSupportedCGIExecutorFlag(supportedExt) << RESET << std::endl;
	paramsList[idx++] = (char *)scriptExec.c_str();
	if (scriptExecFlag.size())
		paramsList[idx++] = (char *)scriptExecFlag.c_str();
	std::string scriptPath;
	scriptPath.append(".");
	scriptPath.append(location->root);
	scriptPath.append("/");
	scriptPath.append(location->cgiIndex);
	paramsList[idx++] = (char *)scriptPath.c_str();
	paramsList[idx++] = NULL;
	char ***envp = NULL;
	envp = new char **[1];
	getEnvVariables(envp);
	cgi_pid = fork();
	if (cgi_pid < 0)
	{
		std::cout << "FORK -1" << std::endl;
		return ;
	}
	if (cgi_pid == 0) // child process
	{
		std::cout << MAGENTA << "\n\n--1.2-- CHILD CLOSE PIPE tubes\n\n" << RESET << std::endl;
		dup2(cgiResponseFds[1], STDOUT_FILENO);
		close(cgiResponseFds[1]);
		close(cgiResponseFds[0]);

		dup2(tubes[0], STDIN_FILENO);
		close(tubes[0]);
		close(tubes[1]);
		execve(paramsList[0], paramsList, *envp);
	}
	else
	{
		clearEnvVariables(envp);
		delete [] envp;
		close(cgiResponseFds[1]);
		std::cout << "---tubes[1](parent) is " << tubes[1] << std::endl;
		sendResponse();
	}
}


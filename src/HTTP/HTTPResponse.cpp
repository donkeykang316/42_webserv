#include "includes/HTTPResponse.hpp"

HTTPResponse::HTTPResponse(int clFd, HTTPRequest &request, std::string filePath) : _request(request)
{
	clientFd = clFd;
	isFulfilled = false;
	cgiResponseFd = 0;
	// fdForCGIIncome = dup(0);
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
		std::cout << "RESPONSE REDIRECT_REQUEST " << response << "|" << std::endl;
		sendResponse();
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
	//  fileExtention = getUriExtention(filePath);
	// std::cout << "fileExtention " << fileExtention << " filePath " << filePath.empty() << " getContentTypeFromDictionary " <<_request.dictionary.getContentTypeFromDictionary(fileExtention) << " " <<_request.dictionary.getContentTypeFromDictionary("html") << std::endl;
	response_headers.append(_request.dictionary.getContentTypeFromDictionary(fileExtention));
	response_headers.append("\r\n");
	// if ()
	if (request.getRequestType() == GET_DIR_LIST)
		content.append(getDirectoryListing(filePath));
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
	if (!request.get_method().compare("GET"))
	{

		sendResponse();

	}
	// 	get();
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

std::string HTTPResponse::getDirectoryListing(std::string filePath)
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
			content.append(ent->d_name);
			content.append("\">\r\n");
			content.append(ent->d_name);
			// printf ("%s\n", ent->d_name);
			content.append("</a>\r\n");
			content.append("</div>\r\n");

		}
		content.append("</div>\r\n");
		closedir (dir);

	}
	else {
  	/* could not open directory */
  		perror ("");
  	// return EXIT_FAILURE;
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

void HTTPResponse::setRequestData(const char *buff)
{
	if (_request.location && _request.location->isCgi)
	{
		std::string buffer = "";
		buffer.append(buff);
		std::cout << "<< SET REQUEST DATA " << _request.isFulfilled<<  std::endl;
		std::cout << buff << std::endl;
		std::cout << ">> SET REQUEST DATA " << std::endl;
		// std::cout << "setRequestData" << std::endl;
		if (buffer.size())
			write(tubes[1], buffer.c_str(), buffer.size() - 1);
		if (_request.isFulfilled)
		{
			pid_t cgi_pid;
				char * paramsList[4];
			paramsList[0] = (char *)"/bin/bash";
			paramsList[1] = (char *)"-c";

			paramsList[2] =(char *)scriptExec.c_str();
			std::cout << "SCRIPT " << scriptExec << std::endl;
			paramsList[3] = NULL;
			char ***envp = NULL;
			envp = new char **[1];
			getEnvVariables(envp);
			std::cout << "*envp[0] "<< (*envp)[0]<< std::endl;
			if ((cgi_pid = fork())== 0) // child process
			{
				dup2(tubes[0], STDIN_FILENO);
				close(tubes[0]);
				close(tubes[1]);
				close(cgiResponseFds[0]);
				dup2(cgiResponseFds[1], STDOUT_FILENO);
				close(cgiResponseFds[1]);
				// dup2 (tubes[1], STDOUT_FILENO);
				// close(tubes[1]);
				// dup2(fdForCGIIncome, STDIN_FILENO);
				// close(fdForCGIIncome);
				// std::cout << "FD CLOSED ON CHILD " << "envp: " << (*envp)[0] << std::endl;
				execve("/bin/bash", paramsList, *envp);
			}
			else
			{
				clearEnvVariables(envp);
				delete [] envp;
				// std::string buffer = request->getBuffer();
				// write(tubes[1], buffer.c_str(), buffer.size() + 1);

				close(tubes[0]);
				close(tubes[1]);
				close(cgiResponseFds[1]);
			}
			response.clear();
			// read status from cgi
			std::string s;
			char ch;
			while (read(cgiResponseFd, &ch, 1) > 0 && ch && ch != '\n')
			{
				s.push_back(ch);
			}
			std::cout << "First string: " << s << std::endl;
			if(!s.compare("Status: OK\r"))
			{

				response.append(_request.get_protocol_v());
				response.append(" 200 OK\r\n");
			}
			s.clear();
			// std::cout << "FD CLOSED " << std::endl;
			// std::string s;
        // char ch;
		// wait(NULL);
			while (read(cgiResponseFd, &ch, 1) > 0)
			{
				// if (ch != 0)
					response.push_back(ch);
				// else
				// {
					// std::cout << s << '\n';
					// response.clear();
				// }
			}
			close(cgiResponseFd);
		std::cout << "<<<< CGI RESPONSE: " << std::endl;
		std::cout << response << std::endl;
		std::cout << ">>>> /CGI RESPONSE: " << std::endl;

		}
	}
	else if (_request.isFulfilled)
		sendResponse();
}

void HTTPResponse::getEnvVariables(char ***envp)
{

	std::map<std::string,std::string> env;
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
	// env["SERVER_PORT"] = _request.
	*envp = new char *[env.size() + 1];
	// envp = &_envp;
	size_t idx = 0;
	for (std::map<std::string, std::string>::iterator envIt = env.begin(); envIt != env.end(); envIt++)
	{
		std::string element = envIt->first + "=" + envIt->second;
		// std::cout << "element idx " << idx << ": " << element << std::endl;
		(*envp)[idx] = new char [element.size() + 1];
		for (size_t i = 0 ; i < element.size(); i++)
			(*envp)[idx][i] = element[i];
		(*envp)[idx][element.size()] = '\0';
		// (*envp)[idx] = (char *)element.c_str();
		// std::cout << "*envp[idx] idx " << idx << ": " << _envp[idx] << std::endl;
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
	if (supportedExt.empty())
	{
		std::cout << "Incorrect data in config for cgi" << std::endl;
		return ;
	}
	// fd = open(//temporary file to record cgi result);
	scriptExec = request->dictionary.getSupportedCGIExecutor(supportedExt);
	scriptExec.append(" ");
	scriptExec.append(".");
	scriptExec.append(filePath);
	// char * paramsCGIList[4];
	// // paramsCGIList = malloc(4 *sizeof(char *));
	// paramsCGIList[0] = (char *)"/bin/bash";
	// paramsCGIList[1] = (char *)"-c";
	// // paramsList[2] = (char *)"php-cgi";
	// // paramsList[3] = (char *)"-q";
	// // paramsList[4] = (char *)"/root/42_school/webserver/cgi-bin/index.php";

	// paramsCGIList[2] =(char *)scriptExec.c_str();
	// std::cout << "SCRIPT " << scriptExec << std::endl;
	// // paramsList[2] =(char *)"ls";
	// paramsCGIList[3] = NULL;
	// paramsCGIList = paramsList;
	// pid_t cgi_pid;
	pipe(tubes);
	// int cgiResponseFds[2];
	pipe(cgiResponseFds);
	cgiResponseFd = cgiResponseFds[0];

	std::cout << "RUNNING CGI " << scriptExec << std::endl;
	// envp = NULL;
	// envp = new char **[1];
	// getEnvVariables(envp);
	// std::cout << "*envp[0] "<< (*envp)[0]<< std::endl;
	// if ((cgi_pid = fork())== 0) // child process
	// {
	// 	dup2(tubes[0], STDIN_FILENO);
	// 	close(tubes[0]);
	// 	close(tubes[1]);
	// 	close(cgiResponseFds[0]);
	// 	dup2(cgiResponseFds[1], STDOUT_FILENO);
	// 	close(cgiResponseFds[1]);
	// 	// dup2 (tubes[1], STDOUT_FILENO);
	// 	// close(tubes[1]);
	// 	// dup2(fdForCGIIncome, STDIN_FILENO);
	// 	// close(fdForCGIIncome);
	// 	// std::cout << "FD CLOSED ON CHILD " << "envp: " << (*envp)[0] << std::endl;
	// 	execve("/bin/bash", paramsCGIList, *envp);
	// }
	// else
	// {
	// 	clearEnvVariables(envp);
	// 	delete [] envp;
	// 	// std::string buffer = request->getBuffer();
	// 	// write(tubes[1], buffer.c_str(), buffer.size() + 1);

	// 	close(tubes[0]);
	// 	close(cgiResponseFds[1]);
	// }
}

void HTTPResponse::sendResponse()
{
    // std::cout << "sendResponse " << response << "|sendResponse" << std::endl;
	// send(clientFd , response.c_str(), response.size(),0);
	//_request.isFulfilled = true;
	// close(clientFd);
}

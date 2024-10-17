#include "HTTPResponse.hpp"

HTTPResponse::HTTPResponse(HTTPRequest &request) : _request(request)
{
	status_code = request.get_status_code();
	if (status_code)
	{
		// make error response
		return;
	}
	else
	{
		// TODO: check on existanse of file that should be send
		status_code = ok;
		status_line.append(_request.get_protocol_v());
		status_line.append(" ");
		std::ostringstream stat_code_str;
		stat_code_str << status_code;
		status_line.append(stat_code_str.str());
		status_line.append(" ");
		status_line.append(get_status_code_msg(status_code));
		status_line.append("\r\n");
		if (_request.headers.find("Connection") == _request.headers.end())
			response_headers.append("Connection: keep-alive\r\n");
		else
		{
			response_headers.append("Connection: ");
			response_headers.append(_request.headers["Connection"]);
			response_headers.append("\r\n");
		}
		if (!_request.get_path().compare("/"))
			response_headers.append("Content-Type: text/html\r\n");
		else if (!_request.get_path().compare("/favicon.ico"))
			response_headers.append("Content-Type: image/x-icon\r\n");
		else if (!_request.get_path().compare("/404.css"))
			response_headers.append("Content-Type: text/css\r\n");
		else
			response_headers.append("Content-Type: text/html\r\n");
		_set_content();
		response.append(status_line);
		response.append(response_headers);
		response.append("Content-Length: ");
		std::ostringstream cont_l;
		cont_l << content.size();
		response.append(cont_l.str());
		response.append("\r\n");
		response.append("\r\n");
		response.append(content);

		if (request.get_method().compare("GET"))
			get();
	}
}

HTTPResponse::~HTTPResponse()
{
}

void HTTPResponse::get()
{
}

void HTTPResponse::_set_content()
{
	std::string filePath;
	// FilePath.clear();
	if (!_request.get_path().compare("/"))
		filePath.append("./www/index.html");
	else if (!_request.get_path().compare("/favicon.ico"))
		filePath.append("./favicon.ico");
	else if (!_request.get_path().compare("/404.css"))
		filePath.append("./www/404.css");
	else
		filePath.append("./www/404.html");
	FILE *fileToRead = std::fopen(filePath.c_str(), "r");
	if (fileToRead == NULL)
	{
		std::cerr << "file open error\n";
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

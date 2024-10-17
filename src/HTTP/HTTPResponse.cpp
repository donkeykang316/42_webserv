#include "includes/HTTPResponse.hpp"

HTTPResponse::HTTPResponse(HTTPRequest &request, std::string filePath) : _request(request)
{
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
	std::cout << "fileExtention " << fileExtention << " filePath " << filePath.empty() << " getContentTypeFromDictionary " <<_request.dictionary.getContentTypeFromDictionary(fileExtention) << " " <<_request.dictionary.getContentTypeFromDictionary("html") << std::endl;
	response_headers.append(_request.dictionary.getContentTypeFromDictionary(fileExtention));
	response_headers.append("\r\n");
	std::cout << "4 RESPONSE " << std::endl;
	_set_content(filePath);
	std::cout << "5 RESPONSE " << std::endl;
	response.append(status_line);
	response.append(response_headers);
	response.append("Content-Length: ");
	std::ostringstream cont_l;
	cont_l << content.size();
	response.append(cont_l.str());
	response.append("\r\n");
	response.append("\r\n");
	std::cout << "6 RESPONSE " << std::endl;
	response.append(content);

	if (request.get_method().compare("GET"))
		get();
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

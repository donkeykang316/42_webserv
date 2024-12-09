#include "includes/Dictionary.hpp"

Dictionary::Dictionary()
{
	init();
}

Dictionary::~Dictionary()
{
}

// Config dictionary

void Dictionary::_setConfigDictionary()
{
	std::string blockNames[] = {"server", "location"};
	for (int i = 0; i < 2; i++)
		configBlockLevels[blockNames[i]] = i;
	std::string serverAttributes[] = {"listen", "server_name", "error_page", "client_max_body_size", "location", "return"};
	std::string locationAttributes[] = {"root", "index", "allow_methods", "client_max_body_size", "error_page", "autoindex", "cgi_pass", "cgi_index", "include", "cgi_param", "return"};
	configServerAttributes = std::set<std::string>(serverAttributes, serverAttributes + sizeof(serverAttributes) / sizeof(serverAttributes[0]));
	configLocationAttributes = std::set<std::string>(locationAttributes, locationAttributes + sizeof(locationAttributes) / sizeof(locationAttributes[0]));

	configVariables["REQUEST_URI"] = "$request_uri";
	std::string cgiExtensions[] = {".php", ".py"};
	for (int i = 0; i < 2; i++)
		supportedCGIExtensions[cgiExtensions[i]] = cgiExtensions->substr(1);
	supportedCGIExecutors["php"] = "/bin/php-cgi";
	supportedCGIExecutors["py"] = "/bin/python3";
}

int	Dictionary::getConfigBlockLevel(std::string blockName)
{
	if (configBlockLevels.find(blockName) == configBlockLevels.end())
		return (-1);
	return (configBlockLevels[blockName]);
}

bool Dictionary::isAttributeInServerDictionary(std::string attributeName)
{
	if (configServerAttributes.find(attributeName) == configServerAttributes.end())
		return (false);
	return (true);
}

bool Dictionary::isAttributeInLocationDictionary(std::string attributeName)
{
	if (configLocationAttributes.find(attributeName) == configLocationAttributes.end())
		return (false);
	return (true);
}

void Dictionary::_setContentTypes()
{
	// text
	contentTypes["html"] = "text/html";
	contentTypes["css"] = "text/css";
	contentTypes["js"] = "text/javascript";
	contentTypes["txt"] = "text/plain";
	// image
	contentTypes["ico"] = "image/x-icon";
	contentTypes["gif"] = "image/gif";
	contentTypes["jpeg"] = "image/jpeg";
	contentTypes["png"] = "image/png";
	contentTypes["svg"] = "image/svg+xml";
}

void Dictionary::_setMethods()
{
	const std::string methodsArr[] = {"GET", "POST", "DELETE"};
	methods = std::set<std::string>(methodsArr, methodsArr + sizeof(methodsArr) / sizeof(methodsArr[0]));
}


void Dictionary::init()
{
	_setConfigDictionary();
	_setMethods();
	_setContentTypes();
}

bool Dictionary::isMethodInDictionary(std::string method)
{
	if (methods.find(method) == methods.end())
		return (false);
	return (true);
}

std::string Dictionary::getContentTypeFromDictionary(std::string type)
{
	if (type.empty())
		return (contentTypes["html"]);
	if (contentTypes.find(type) == contentTypes.end())
		return ("application/octet-stream");
	return (contentTypes[type]);
}

std::string Dictionary::getConfigVariable(std::string alias)
{
	if (configVariables.find(alias) == configVariables.end())
		return ("");
	return (configVariables[alias]);
}

std::string Dictionary::getSupportedCGIExtension(std::string ext)
{
	if (this->supportedCGIExtensions.find(ext) != this->supportedCGIExtensions.end())
		return (this->supportedCGIExtensions[ext]);
	else
		return ("");
}

std::string Dictionary::getSupportedCGIExecutor(std::string ext)
{
	if (this->supportedCGIExecutors.find(ext) != this->supportedCGIExecutors.end())
		return (this->supportedCGIExecutors[ext]);
	else
		return ("");
}

std::string Dictionary::getSupportedCGIExecutorFlag(std::string ext)
{
	if (this->supportedCGIExecutorsFlag.find(ext) != this->supportedCGIExecutorsFlag.end())
		return (this->supportedCGIExecutorsFlag[ext]);
	else
		return ("");
}

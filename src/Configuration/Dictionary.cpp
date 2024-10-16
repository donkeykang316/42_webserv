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
	std::string serverAttributes[] = {"listen", "server_name", "error_page", "client_max_body_size", "location"};
	std::string locationAttributes[] = {"root", "index", "allow_methods", "error_page", "autoindex", "cgi_pass", "cgi_index", "include", "cgi_param"};
	configServerAttributes = std::set<std::string>(serverAttributes, serverAttributes + sizeof(serverAttributes) / sizeof(serverAttributes[0]));
	configLocationAttributes = std::set<std::string>(locationAttributes, locationAttributes + sizeof(locationAttributes) / sizeof(locationAttributes[0]));
}

int	Dictionary::getConfigBlockLevel(std::string blockName)
{
	if (configBlockLevels.find(blockName) == configBlockLevels.end())
		return (-1);
	return (configBlockLevels[blockName]);
}

bool Dictionary::isAttributeInServerDictionary(std::string attribureName)
{
	if (configServerAttributes.find(attribureName) == configServerAttributes.end())
		return (false);
	return (true);
}

bool Dictionary::isAttributeInLocationDictionary(std::string attribureName)
{
	if (configLocationAttributes.find(attribureName) == configLocationAttributes.end())
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
}

void Dictionary::_setMethods()
{
	const std::string methodsArr[] = {"GET", "POST", "DELETE"};
	methods = std::set<std::string>(methodsArr, methodsArr + sizeof(methodsArr) / sizeof(methodsArr[0]));
}

// void Dictionary::_setLocationAttributes()
// {
// 	const std::string attributes[] = {"root", "index", "allow_methods", "error_page"};
// }

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


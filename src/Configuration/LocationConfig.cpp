#include "includes/LocationConfig.hpp"

// Location
LocationConfig::LocationConfig(Dictionary &dict, std::map<std::string, std::vector<std::string> > serverErrPages, int clientMaxBodySizeGlobal) : dictionary(dict)
{
	resetToDefault();
	clientMaxBodySize = clientMaxBodySizeGlobal;
	std::map<std::string, std::vector<std::string> >::iterator itErrPage;

	for (itErrPage = serverErrPages.begin(); itErrPage != serverErrPages.end(); itErrPage++)
	{
		std::vector<std::string> second = itErrPage->second;
		errorPages[itErrPage->first] = second;
	}
}
LocationConfig &LocationConfig::operator=(const LocationConfig &rhs)
{
	resetToDefault();
	this->dictionary = rhs.dictionary;
	this->clientMaxBodySize = rhs.clientMaxBodySize;

	std::map<std::string, std::vector<std::string> >::const_iterator itErrPage;

	for (itErrPage = rhs.errorPages.begin(); itErrPage != rhs.errorPages.end(); itErrPage++)
	{
		std::vector<std::string> second = itErrPage->second;
		this->errorPages[itErrPage->first] = second;
	}
	return (*this);
}

LocationConfig::~LocationConfig()
{
}
void LocationConfig::resetToDefault()
{
	this->modifier.clear();
	this->strictMatch = false;
	this->uri.clear();
	this->allowedMethods.clear();
	this->root.clear();
	this->index.clear();
	this->autoindex = false;
	this->clientMaxBodySize = -1;

	this->isCgi = false;
	this->cgiPass.clear();
	this->cgiIndex.clear();
	this->cgiInclude.clear();
	this->cgiParams.clear();
	this->errorPages.clear();
	this->redirection = std::make_pair("","");
	this->regexValue.clear();
}
void LocationConfig::setUri(std::vector<std::string> vector)
{
	size_t idx = 1;
	if (vector[idx].size() == 1 && (vector[idx][0] == '=' || vector[idx][0] == '~'))
	{
		modifier = vector[idx];
		strictMatch = (vector[idx][0] == '=');
		idx++;
	}
	if (vector.size() != (idx + 2))
	{
		modifier.clear();
		std::cout << "Invalid location data" << std::endl;
		return;
	}
	if (vector.size() != (idx + 2))
	{
		modifier.clear();
		return;
	}
	if (modifier[0] != '~' && vector[idx][0] != '/')
	{
		uri.clear();
		std::cout << "Invalid location uri data. URI should start with \"/\" symbol" << std::endl;
		return ;
	}
	uri = vector[idx];
	if (modifier[0] == '~')
	{
		size_t start = 0;
		size_t end = uri.size();
		if (uri.find('^') == 0)
			start++;
		if (uri.find_last_of('$') == (uri.size() - 1))
			end--;
		regexValue.append(uri.substr(start, end));
		size_t i = 0;
		while ((i = regexValue.find_first_of('\\', i)) != std::string::npos)
		{
			regexValue.erase(regexValue.begin() + i);
			i++;
		}
		std::cout << "REGEX: " << regexValue << std::endl;
	}
}
void LocationConfig::setRoot(std::vector<std::string> vector)
{
	if (!isValidOneValue(vector))
	{
		root.empty();
		return;
	}
	size_t v1Size = vector[1].size();
	if (vector[1][0] != '/' || (v1Size > 1 && vector[1][v1Size - 1] == '/'))
	{
		uri.clear();
		std::cout << "Invalid location root data. Root should start with \"/\" symbol and must not have symbol \"/\"  at the end." << std::endl;
		return ;
	}
	root = vector[1];
	size_t rootSize = root.size();
	if (rootSize > 1 && root[rootSize - 1] == '/')
		root.erase(rootSize - 1);
}

void LocationConfig::setIndex(std::vector<std::string> vector)
{
	if (!isValidOneValue(vector))
	{
		index.empty();
		return;
	}
	index = vector[1];
}

void LocationConfig::setAutoIndex(std::vector<std::string> vector)
{
	if (!isValidOneValue(vector))
	{
		autoindex = false;
		return;
	}
	if (vector[1].compare("on") && vector[1].compare("off"))
	{
		std::cout << "Autoindex can accept only \"on\" and \"off\" values. Autoindex is forcibly set to false." << std::endl;
		return ;
	}
	autoindex = vector[1].compare("off");
}

void LocationConfig::setClientMaxBodySize(std::vector<std::string> vector)
{
	if (!isValidOneDigitValue(vector))
	{
		// clientMaxBodySize = -1;
		return;
	}
	int i;
	std::istringstream(vector[1]) >> i;
	clientMaxBodySize = i;
}

void LocationConfig::setAllowedMethods(std::vector<std::string> vector)
{
	size_t size = vector.size();
	for (size_t i = 1; i < size; i++)
	{
		if (dictionary.isMethodInDictionary(vector[i]))
			allowedMethods.insert(vector[i]);
		else
			std::cout << "Method " << vector[i] << " is not supported by server. This method will be ignored" << std::endl;
	}
}

void LocationConfig::setCgiPass(std::vector<std::string> vector)
{
	isCgi = true;
	if (!isValidOneValue(vector))
	{
		cgiPass.empty();
		return;
	}
	cgiPass = vector[1];
}

void LocationConfig::setCgiIndex(std::vector<std::string> vector)
{
	isCgi = true;
	if (!isValidOneValue(vector))
	{
		cgiIndex.empty();
		return;
	}
	cgiIndex = vector[1];
}

void LocationConfig::setCgiInclude(std::vector<std::string> vector)
{
	isCgi = true;
	if (!isValidOneValue(vector))
	{
		cgiInclude.empty();
		return;
	}
	cgiInclude = vector[1];
}

void LocationConfig::setCgiParams(std::vector<std::string> vector)
{
	isCgi = true;
	if (vector.size() != 3)
	{
		std::cout << "Incorrect format fo CGI parameters" << std::endl;
		return;
	}
	cgiParams[vector[1]] = vector[2];
}

void LocationConfig::fillAttributes(std::vector<std::string> confLineVector, Dictionary &dictionary)
{
	if (!dictionary.isAttributeInLocationDictionary(confLineVector[0]))
	{
		std::cout << "Attribute of location \"" << confLineVector[0] << "\" not supported." << std::endl;
		return;
	}
	std::string attributeName = confLineVector[0];
	if (!attributeName.compare("root"))
		setRoot(confLineVector);
	else if (!attributeName.compare("index"))
		setIndex(confLineVector);
	else if (!attributeName.compare("autoindex"))
		setAutoIndex(confLineVector);
	else if (!attributeName.compare("error_page"))
		addErrorPages(confLineVector);
	else if (!attributeName.compare("allow_methods"))
		setAllowedMethods(confLineVector);
	else if (!attributeName.compare("cgi_pass"))
		setCgiPass(confLineVector);
	else if (!attributeName.compare("cgi_index"))
		setCgiIndex(confLineVector);
	else if (!attributeName.compare("include"))
		setCgiInclude(confLineVector);
	else if (!attributeName.compare("cgi_param"))
		setCgiParams(confLineVector);
	else if (!attributeName.compare("return"))
		setRedirection(confLineVector);
	else if (!attributeName.compare("client_max_body_size"))
		setClientMaxBodySize(confLineVector);
}

bool LocationConfig::isValid()
{
	if (!isCgi && (uri.empty() || root.empty()))
		return (false);
	return (true);
}

bool	LocationConfig::isMethodAllowed(std::string method)
{
	if (allowedMethods.find(method) != allowedMethods.end())
		return (true);
	return (false);
}

std::string LocationConfig::getRegexValue()
{
	return(this->regexValue);
}


std::string LocationConfig::getCgiExtentionFromUri(std::string uri)
{
	size_t pos = uri.find_last_of('.');
	if (pos == std::string::npos || uri.size() - pos < 3)
		return ("");
	size_t endPos = uri.substr(pos + 1).find_first_not_of("abcdefghijklmnopqrstuvwxyz");
	return (uri.substr(pos, endPos));
}

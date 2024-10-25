#include "includes/ServerConfig.hpp"

// ServerConfig.
ServerConfig::ServerConfig(Dictionary &dictionary): _dictionary(dictionary)
{
	resetToDefault();
}

ServerConfig & ServerConfig::operator=(const ServerConfig &rhs)
{
	(void) rhs;
	this->resetToDefault();
	std::cout << "SERVERCONFIG OPERATOR =" << std::endl;
	return (*this);
}

ServerConfig::~ServerConfig()
{
}

void ServerConfig::resetToDefault()
{
	this->listen.clear();
	this->serverNameAliases.clear();
	this->clientMaxBodySize = -1;
	this->index.clear();
	this->locations.clear();
	this->errorPages.clear();
	this->redirection = std::make_pair("","");
	// this->supportedCgiExtentions.clear();
	this->regexLocations.clear();
}

void ServerConfig::setListen(std::vector<std::string> vector)
{

	if (!isValidOneDigitValue(vector))
	{
		listen.clear();
		return;
	}
	listen.insert(vector[1]);
}

void ServerConfig::setServerName(std::vector<std::string> vector)
{
	// if (!isValidOneValue(vector))
	// {
		// serverNameAliases.clear();
		// return;
	// }
	if (vector.size() < 2)
	{
		std::cout << "Instruction \"" << vector[0] << "\" must have at least one value" << std::endl;
		return ;
	}
	for (size_t i = 1; i < vector.size(); i++)
		serverNameAliases.insert(vector[i]);
}

void ServerConfig::setClientMaxBodySize(std::vector<std::string> vector)
{
	if (!isValidOneDigitValue(vector))
	{
		clientMaxBodySize = 0;
		return;
	}
	int i;
	std::istringstream(vector[1]) >> i;
	clientMaxBodySize = i;
}

void ServerConfig::setIndex(std::vector<std::string> vector)
{
	if (!isValidOneValue(vector))
	{
		index = "";
		return;
	}
	index = vector[1];
}

void ServerConfig::fillAttributes(std::vector<std::string> confLineVector)
{
	if (!_dictionary.isAttributeInServerDictionary(confLineVector[0]))
	{
		std::cout << "Attribute of server \"" << confLineVector[0] << "\" not supported." << std::endl;
		return;
	}
	std::string attributeName = confLineVector[0];
	if (!attributeName.compare("listen"))
		setListen(confLineVector);
	else if (!attributeName.compare("server_name"))
		setServerName(confLineVector);
	else if (!attributeName.compare("client_max_body_size"))
		setClientMaxBodySize(confLineVector);
	else if (!attributeName.compare("error_page"))
		addErrorPages(confLineVector);
	else if (!attributeName.compare("return"))
		setRedirection(confLineVector);
}

bool ServerConfig::isValid()
{
	if (listen.empty() || serverNameAliases.empty())
		return (false);
	return (true);
}

LocationConfig *ServerConfig::getRegexLocation(std::string path)
{
	LocationConfig *currRegexLocation = NULL;
	for (std::vector<LocationConfig>::iterator it = this->regexLocations.begin(); it != this->regexLocations.end(); it++)
	{
		std::string regexVal = it->getRegexValue();
		if (path.find(regexVal) == std::string::npos)
			continue;
		size_t pos = path.find_last_of(regexVal);
		if (pos == std::string::npos)
			continue;
		std::cout << "1.________________regexVal is in uri " << regexVal << std::endl;
		std::string uri = (it)->uri;
		std::cout << "URI: " << uri  << " path: " << path <<  std::endl;

		if (uri[0] == '^' && !pos)
			continue;
		std::cout << "2.________________regexVal match start " << regexVal << std::endl;
		std::cout << "2.1.________________ " << pos << " " << pos + regexVal.size() << " " << path.size() << std::endl;

		if (uri[uri.size() - 1] == '$' && (pos + 1 != path.size()))
			continue;
		std::cout << "3.________________regexVal match end " << regexVal << std::endl;

		std::cout << "________________regexVal " << regexVal << std::endl;
		return (&(*it));
	}
	return (currRegexLocation);
}


LocationConfig *ServerConfig::getLocation(std::string path)
{
	LocationConfig *currLocation = NULL;

	currLocation = getRegexLocation(path);
	if (currLocation)
		return (currLocation);
	std::string filePath = path;
	std::vector<std::string> pathVector;
	pathVector.clear();
	while(!filePath.empty())
	{
		pathVector.push_back(filePath);
		std::cout << filePath << std::endl;
		size_t slashIdx = filePath.find_last_of('/');
		if (slashIdx == filePath.npos)
			break ;
		filePath.erase(slashIdx);
	}
	if (path[0] == '/')
		pathVector.push_back("/");
	int	matchOnLevel = pathVector.size();

	for(std::vector<LocationConfig>::iterator it = locations.begin(); it < locations.end(); it++)
	{
		if (it->strictMatch && it->uri.compare(path))
			continue;
		for (int i = 0; i < matchOnLevel; i++)
		{
			if (!it->uri.compare(pathVector[i]) && i < matchOnLevel)
			{
				matchOnLevel = i;
				currLocation = &(*it);
			}
		}

		if (!matchOnLevel)
			break;

	}
	return (currLocation);
}

LocationConfig *ServerConfig::getSameLocation(std::string path)
{
	std::vector<LocationConfig>::iterator it = locations.begin();
	while(it != locations.end())
	{
		if (!it->uri.compare(path))
			return (&(*it));
		it++;
	}
	return (NULL);
}

LocationConfig *ServerConfig::getSameRegexLocation(std::string path)
{
	std::vector<LocationConfig>::iterator it = regexLocations.begin();
	while(it != regexLocations.end())
	{
		if (!it->uri.compare(path))
			return (&(*it));
		it++;
	}
	return (NULL);
}


std::set<std::string> ServerConfig::getServerNameAliases()
{
	return (this->serverNameAliases);
}
std::set<std::string> ServerConfig::getListenPorts()
{
	return (this->listen);
}

void ServerConfig::addLocation(LocationConfig location)
{
	if (!location.modifier.compare("~"))
	{
		LocationConfig *sameRegexLocation = this->getSameRegexLocation(location.uri);
		if (sameRegexLocation)
			sameRegexLocation = &location;
		else
			regexLocations.push_back(location);
		return ;
	}
	LocationConfig *sameLocation = this->getSameLocation(location.uri);
	if (sameLocation)
		sameLocation = &location;
	else
		this->locations.push_back(location);
	// if (!location.isCgi)
	// 	return ;
	// std::string cgiExtention = location.uri;
	// cgiExtention.erase(0, cgiExtention.find_first_of('.'));
	// cgiExtention.erase(cgiExtention.find_first_of('$'));
	// supportedCgiExtentions[cgiExtention] = location.uri;
	// add regex location
	// if (!location.modifier.compare("~"))

}


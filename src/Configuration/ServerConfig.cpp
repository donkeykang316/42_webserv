#include "includes/ServerConfig.hpp"

// ServerConfig.
ServerConfig::ServerConfig()
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

void ServerConfig::fillAttributes(std::vector<std::string> confLineVector, Dictionary &dictionary)
{
	if (!dictionary.isAttributeInServerDictionary(confLineVector[0]))
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
}

bool ServerConfig::isValid()
{
	if (listen.empty() || serverNameAliases.empty())
		return (false);
	return (true);
}

LocationConfig *ServerConfig::getLocation(std::string path)
{
	std::string filePath = path;
	std::vector<std::string> pathVector;
	LocationConfig *currLocation = NULL;
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

// std::set<std::string> ServerConfig
std::set<std::string> ServerConfig::getServerNameAliases()
{
	return (this->serverNameAliases);
}
std::set<std::string> ServerConfig::getListenPorts()
{
	return (this->listen);
}

#ifndef CONFIGURATION_H
# define CONFIGURATION_H

# include <fstream>
#include "../../HTTP/includes/HTTP.h"
# include "Dictionary.hpp"
# include "AAttribute.hpp"
# include "LocationConfig.hpp"
# include "ServerConfig.hpp"
# include <string>
# include <vector>
# include <stack>
#include <algorithm>


class Configuration
{
private:
	const char *configFile;
public:
	Dictionary dictionary;
	int blockLayer;
	int lineNb;
	std::vector<ServerConfig> servers;
	Configuration(const char *path);
	~Configuration();
	void parseConfig();
	void printConfigurationData();
};

#endif

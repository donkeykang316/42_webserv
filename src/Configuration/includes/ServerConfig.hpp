#ifndef SERVERCONFIG_H
# define SERVERCONFIG_H

# include "LocationConfig.hpp"
# include <sstream>
# include <algorithm>
# include <unistd.h>


class ServerConfig: public Attribute
{
private:
	Dictionary &_dictionary;
	void	setListen(std::vector<std::string> vector);
	void	setServerName(std::vector<std::string> vector);
	void	setClientMaxBodySize(std::vector<std::string> vector);
	void	setIndex(std::vector<std::string> vector);
	void	resetToDefault();
	std::set<std::string> listen;
	std::set<std::string> serverNameAliases;
	// std::map<std::string,std::string> supportedCgiExtentions;
	std::vector<LocationConfig> regexLocations;

public:
	int clientMaxBodySize;
	std::string index;
	std::vector<LocationConfig> locations;

	ServerConfig(Dictionary &dictionary);
	ServerConfig & operator=(const ServerConfig &rhs);
	~ServerConfig();
	void	fillAttributes(std::vector<std::string> confLineVector);
	bool	isValid();
	LocationConfig *getSameLocation(std::string path);
	LocationConfig *getSameRegexLocation(std::string path);
	std::set<std::string> getServerNameAliases();
	std::set<std::string> getListenPorts();
	void addLocation(LocationConfig location);

	LocationConfig *getRegexLocation(const std::string &path);
	LocationConfig *getLocation(std::string path);

};


#endif

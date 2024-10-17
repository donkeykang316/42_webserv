#ifndef SERVERCONFIG_H
# define SERVERCONFIG_H

# include "LocationConfig.hpp"
# include <sstream>
# include <algorithm>


class ServerConfig: public Attribute
{
private:
	// bool	is_valid;
	void	setListen(std::vector<std::string> vector);
	void	setServerName(std::vector<std::string> vector);
	void	setClientMaxBodySize(std::vector<std::string> vector);
	void	setIndex(std::vector<std::string> vector);
	void	resetToDefault();
	std::set<std::string> listen;
	std::set<std::string> serverNameAliases;

public:
	int clientMaxBodySize;
	std::string index;
	std::vector<LocationConfig> locations;

	ServerConfig();
	ServerConfig & operator=(const ServerConfig &rhs);
	~ServerConfig();
	void	fillAttributes(std::vector<std::string> confLineVector, Dictionary &dictionary);
	bool	isValid();
	LocationConfig *getLocation(std::string path);
	std::set<std::string> getServerNameAliases();
	std::set<std::string> getListenPorts();
};


#endif

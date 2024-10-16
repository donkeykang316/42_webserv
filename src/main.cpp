#include "Configuration/includes/Configuration.hpp"
#include "WebServer/includes/WebServer.hpp"

int main(int argc, char *argv[])
{
if (argc < 2)
{
	std::cout << "Server requires a configuration file" << std::endl;
	return(false);
}

Configuration config(argv[1]);
config.printConfigurationData();
config.start();
// std::map<std::string, int> listenSockets;
// std::map<std::string, std::set<std::string> > defaultServerNames;
// std::map<std::string, std::set<std::string> > otherServerNames;

// listenSockets.clear();
// for (std::vector<ServerConfig>::iterator it =config.servers.begin(); it != config.servers.end(); it++)
// {
// 	WebServer webserver(*it, config.dictionary);
// }
}

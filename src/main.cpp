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
config.start();
}

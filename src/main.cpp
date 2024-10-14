#include "Server.hpp"
#include "ConfParser.hpp"

int	main(int ac, char** av) {
	(void)ac;
	(void)av;

	if (ac != 2) {
		std::cerr << "Usage: ./webserv <config_file>" << std::endl;
		return 1;
	}

	ConfParser	parser(av[1]);
	//parser.printConf();

	Server	Server;
}

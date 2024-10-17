#include "includes/Configuration.hpp"

// Configuration
Configuration::Configuration(const char *path) : configFile(path)
{
	blockLayer = 0;
	lineNb = 0;
	servers.clear();

	parseConfig();
}

Configuration::~Configuration()
{
}

void Configuration::parseConfig()
{
	std::stack<std::string> blockInProgressStack;
	std::string blockStart = "{";
	std::string blockEnd = "}";

	std::ifstream nameFileOut;
	nameFileOut.open(configFile);

	std::string configLine;
	std::vector<std::string> confLineVector;

	ServerConfig server;
	LocationConfig location(dictionary, server.errorPages, server.clientMaxBodySize);

	while (std::getline(nameFileOut, configLine))
	{
		lineNb++;

		configLine.erase(0, configLine.find_first_not_of(" \r\t\n"));
		if (configLine.size() == 0 || configLine[0] == '#')
			continue;
		while (configLine.size())
		{
			std::string configLineElem = configLine.substr(0, configLine.find_first_of(" \r\t\n"));
			configLine.erase(0, configLineElem.size());
			if (configLineElem[0] == '#')
			{
				break;
			}
			confLineVector.push_back(configLineElem);
			configLine.erase(0, configLine.find_first_not_of(" \r\t\n"));
			bool isBlockStart = !configLineElem.compare(blockStart);
			bool isBlockEnds = !configLineElem.compare(blockEnd);
			int isNotAttributeEnd = 0 + configLineElem.at(configLineElem.size() - 1) - ';';

			// check error in structure cases:
			//	1. closing } without opening one
			//	2. incorrect level of block
			//	3. attributes are out of scope
			if ((isBlockEnds && blockLayer == 0) || (isBlockStart && blockLayer != dictionary.getConfigBlockLevel(confLineVector[0])) || (!blockLayer && !isNotAttributeEnd))
			{
				std::cout << "Incorrect configuration struct on line " << lineNb << std::endl;
				exit(1);
			}
			if (isBlockEnds)
			{
				switch (dictionary.getConfigBlockLevel(blockInProgressStack.top()))
				{
				case 1:
					if (location.isValid())
						server.locations.push_back(location);
					else
					{
						std::cout << "ERROR WITH VALIDATION OF LOCATION" << std::endl;
						exit(1);
					}
					break;
				case 0:
					if (server.isValid())
						this->servers.push_back(server);
					else
					{
						std::cout << "ERROR WITH VALIDATION OF SERVER" << std::endl;
						exit(1);
					}
					server = ServerConfig();
				default:
					break;
				}
				blockLayer--;
				blockInProgressStack.pop();
			}
			if (isBlockStart)
			{
				if (blockLayer == 0 && confLineVector.size() > 2)
				{
					std::cout << "Invalid instruction for " << confLineVector[0] << " on line " << lineNb << ". Extra parameters" << std::endl;
					exit(1);
				}
				else if (blockLayer == 1)
				{
					location = LocationConfig(dictionary, server.errorPages, server.clientMaxBodySize);
					location.setUri(confLineVector);
				}
				blockLayer++;
				blockInProgressStack.push(confLineVector[0]);
			}

			if (!isNotAttributeEnd)
			{
				int lastElemIdx = confLineVector.size() - 1;
				if (confLineVector[lastElemIdx].size() == 1)
					confLineVector.pop_back();
				else
					confLineVector[lastElemIdx].erase(confLineVector[lastElemIdx].find_last_of(';'));
				switch (dictionary.getConfigBlockLevel(blockInProgressStack.top()))
				{
				case 0:
					server.fillAttributes(confLineVector, dictionary);
					break;
				case 1:
					location.fillAttributes(confLineVector, dictionary);
				default:
					break;
				}
			}
			if (isBlockStart || isBlockEnds || !isNotAttributeEnd)
				confLineVector.clear();
		}
	}
}

void Configuration::printConfigurationData(){
	int serverIdx = 0;
	int locationsIdx = 0;

	std::vector<ServerConfig>::iterator currServer = servers.begin();
	while (currServer != servers.end())
	{
		std::cout << "SERVER number " << serverIdx << std::endl;
		std::cout << "--- listen: ";
		for (std::set<std::string>::iterator port = currServer->listen.begin(); port != currServer->listen.end(); port++)
		std::cout << *port << " ";
		std::cout << std::endl;
		std::cout << "--- index: " << currServer->index << std::endl;
		std::cout << "--- serverName: " << currServer->serverName << std::endl;
		std::cout << "--- clientMaxBodySize: " << currServer->clientMaxBodySize << std::endl;
		std::cout << "--- ERROR PAGES: "<< std::endl;
		std::map<std::string, std::vector<std::string> >::iterator itErr;

		for (itErr = currServer->errorPages.begin(); itErr != currServer->errorPages.end(); itErr++)
		{
			std::cout << "------ " << itErr->first    // string (key)
					<< ':';
			std::vector<std::string>::iterator itErrPage;
			for (itErrPage = itErr->second.begin(); itErrPage != itErr->second.end(); itErrPage++)
					std::cout << *itErrPage << " ";   // string's value
			std::cout << std::endl;
		}
		serverIdx++;
		std::vector<LocationConfig>::iterator currLocation = currServer->locations.begin();
		while (currLocation != currServer->locations.end())
		{
		std::cout << std::endl << "--- LOCATION number " << locationsIdx << std::endl;
		std::cout << "------ is CGI: " << currLocation->isCgi << std::endl;
		std::cout << "------ uri: " << currLocation->uri << std::endl;
		std::cout << "------ modifier: " << currLocation->modifier << std::endl;
		std::cout << "------ index: " << currLocation->index << std::endl;
		std::cout << "------ cgiPass: " << currLocation->cgiPass << std::endl;
		std::cout << "------ cgiIndex: " << currLocation->cgiIndex << std::endl;
		std::cout << "------ cgiInclude: " << currLocation->cgiInclude << std::endl;
		std::cout << "------ CGIParams: " << std::endl;
		for(std::map<std::string, std::string>::iterator it = currLocation->cgiParams.begin(); it != currLocation->cgiParams.end(); it++)
		{
			std::cout << "--------- " << it->first    // string (key)
					<< ':' << it->second << std::endl;
		}
		std::cout << "------ ALLOWED METHODS: "<< std::endl;
		std::set<std::string>::iterator itMethod;
		std::cout << "[ ";
		for (itMethod = currLocation->allowedMethods.begin(); itMethod != currLocation->allowedMethods.end(); itMethod++)
		{
			std::cout << itMethod->data() << " ";
		}
		std::cout << "]" << std::endl;
		std::cout << "------ ERROR PAGES: "<< std::endl;

		for (itErr = currLocation->errorPages.begin(); itErr != currLocation->errorPages.end(); itErr++)
		{
			std::cout << "--------- " << itErr->first    // string (key)
					<< ':';
			std::vector<std::string>::iterator itErrPage;
			for (itErrPage = itErr->second.begin(); itErrPage != itErr->second.end(); itErrPage++)
					std::cout << *itErrPage << " ";   // string's value
			std::cout << std::endl;
		}

			currLocation++;
			locationsIdx++;
		}
		currServer++;
		locationsIdx = 0;
	}
};


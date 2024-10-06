#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <cstdlib>
#include "ServerConf.hpp"
#include "Location.hpp"

class ConfParser {
public:
	std::map<int, ServerConf> _servers;

	ConfParser(char* filePath);
	~ConfParser();

	void printConf();
};
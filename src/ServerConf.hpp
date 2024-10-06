#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include "Location.hpp"

class ServerConf {
public:
    std::map<std::string, std::string> _properties;
    std::map<std::string, Location> _locations;

    ServerConf() {}
    ~ServerConf() {}
};
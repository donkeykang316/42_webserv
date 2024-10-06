#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>

class Location {
public:
    std::map<std::string, std::string> _properties;

    Location() {}
    ~Location() {}
};
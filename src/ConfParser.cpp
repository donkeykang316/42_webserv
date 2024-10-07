#include "ConfParser.hpp"

ConfParser::ConfParser(char* filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error opening config file: " << filePath << std::endl;
        std::exit(1);
    }

    std::string line;
    int lineNumber = 0;
    int serverIndex = -1;
    bool inServerBlock = false;
    bool inLocationBlock = false;
    ServerConf currentServer;
    Location currentLocation;
    std::string currentLocationPath;
    while (std::getline(file, line))
    {
        lineNumber++;

        // Skip empty comments and whitespaces
        std::string::size_type commentPos = line.find('#');
        if (commentPos != std::string::npos)
            line.erase(commentPos);

        std::string::size_type start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos)
            continue;
        std::string::size_type end = line.find_last_not_of(" \t\r\n");
        std::string trimmedLine = line.substr(start, end - start + 1);

        //std::cout << "Line: " << line << std::endl;

        // Handle closing braces
        if (trimmedLine == "}") {
            if (inLocationBlock) {
                // End of location block
                inLocationBlock = false;
                currentServer._locations[currentLocationPath] = currentLocation;
                currentLocation = Location();
                currentLocationPath = "";
            } else if (inServerBlock) {
                // End of server block
                inServerBlock = false;
                _servers[serverIndex] = currentServer;
                currentServer = ServerConf();
            } else {
                std::cerr << "Unexpected closing brace at line " << lineNumber << std::endl;
                std::exit(1);
            }
            continue;
        }


        // handle server block
        if (trimmedLine == "server {") {
            if (inServerBlock) {
                std::cerr << "Nested server blocks are not allowed at line " << lineNumber << std::endl;
                return;
            }
            inServerBlock = true;
            serverIndex++;
            currentServer = ServerConf();
            continue;
        }

        //handle location block
        if (trimmedLine.substr(0, 8) == "location") {
            if (!inServerBlock) {
                std::cerr << "Location block outside server block at line " << lineNumber << std::endl;
                return;
            }
            inLocationBlock = true;
            currentLocation = Location();
            std::string::size_type pos = trimmedLine.find('{');
            if (pos == std::string::npos) {
                std::cerr << "Expected '{' after location at line " << lineNumber << std::endl;
                return;
            }
            std::string locationDef = trimmedLine.substr(0, pos);
            std::string::size_type spacePos = locationDef.find(' ');
            if (spacePos == std::string::npos) {
                std::cerr << "Invalid location definition at line " << lineNumber << std::endl;
                return;
            }
            currentLocationPath = locationDef.substr(spacePos + 1);
            // Trim trailing whitespace
            end = currentLocationPath.find_last_not_of(" \t\r\n");
            if (end != std::string::npos) {
                currentLocationPath = currentLocationPath.substr(0, end + 1);
            }
            continue;
        }

        // check if line ends with ';'
        if (trimmedLine[trimmedLine.size() - 1] != ';') {
            std::cerr << "Expected ';' at end of directive at line " << lineNumber << std::endl;
            return;
        }

        // Remove ';' at the end
        trimmedLine = trimmedLine.substr(0, trimmedLine.size() - 1);

        // Split directive into key and value
        std::istringstream iss(trimmedLine);
        std::string key;
        if (!(iss >> key)) {
            std::cerr << "Invalid directive at line " << lineNumber << std::endl;
            return;
        }
        std::string value;
        std::getline(iss, value);

        // Trim leading whitespace from value
        start = value.find_first_not_of(" \t\r\n");
        if (start != std::string::npos) {
            value = value.substr(start);
        } else {
            value = "";
        }

        if (inLocationBlock) {
            currentLocation._properties[key] = value;
        } else if (inServerBlock) {
            currentServer._properties[key] = value;
        } else {
            std::cerr << "Directive outside of block at line " << lineNumber << std::endl;
            return;
        }
    }
    file.close();
}

ConfParser::~ConfParser() {}

void ConfParser::printConf() {
    for (std::map<int, ServerConf>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
        std::cout << "Server:           " << it->first << std::endl;
        for (std::map<std::string, std::string>::iterator it2 = it->second._properties.begin(); it2 != it->second._properties.end(); ++it2) {
            std::cout << it2->first << ":           " << it2->second << std::endl;
        }
        for (std::map<std::string, Location>::iterator it2 = it->second._locations.begin(); it2 != it->second._locations.end(); ++it2) {
            std::cout << "Location:             " << it2->first << std::endl;
            for (std::map<std::string, std::string>::iterator it3 = it2->second._properties.begin(); it3 != it2->second._properties.end(); ++it3) {
                std::cout << it3->first << ":           " << it3->second << std::endl;
            }
        }
    }
}
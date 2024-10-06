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
    ServerConf currentServer;
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

        std::cout << "Line: " << line << std::endl;

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
        if (trimmedLine == "}") {
            if (!inServerBlock) {
                std::cerr << "Unexpected '}' at line " << lineNumber << std::endl;
                return;
            }
            inServerBlock = false;
            _servers[serverIndex] = currentServer;
            continue;
        }
    }
}

ConfParser::~ConfParser() {}

void ConfParser::printConf() {
    for (std::map<int, ServerConf>::iterator it = _servers.begin(); it != _servers.end(); ++it) {
        std::cout << "Server " << it->first << std::endl;
        for (std::map<std::string, std::string>::iterator it2 = it->second._properties.begin(); it2 != it->second._properties.end(); ++it2) {
            std::cout << it2->first << ": " << it2->second << std::endl;
        }
        for (std::map<std::string, Location>::iterator it2 = it->second._locations.begin(); it2 != it->second._locations.end(); ++it2) {
            std::cout << "Location " << it2->first << std::endl;
            for (std::map<std::string, std::string>::iterator it3 = it2->second._properties.begin(); it3 != it2->second._properties.end(); ++it3) {
                std::cout << it3->first << ": " << it3->second << std::endl;
            }
        }
    }
}
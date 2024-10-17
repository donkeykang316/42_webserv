#ifndef LOCATIONCONFIG_H
# define LOCATIONCONFIG_H

# include <vector>
# include <string>
# include <set>
# include <map>
# include <iostream>
# include <sstream>
# include "AAttribute.hpp"
# include "Dictionary.hpp"

class LocationConfig: public Attribute
{
	private:
		void setRoot(std::vector<std::string> vector);
		void setIndex(std::vector<std::string> vector);
		void setAutoIndex(std::vector<std::string> vector);
		void setAllowedMethods(std::vector<std::string> vector);
		void setClientMaxBodySize(std::vector<std::string> vector);

		void setCgiPass(std::vector<std::string> vector);
		void setCgiIndex(std::vector<std::string> vector);
		void setCgiInclude(std::vector<std::string> vector);
		void setCgiParams(std::vector<std::string> vector);
		void resetToDefault();
	public:
		Dictionary &dictionary;
		std::string modifier;
		bool	strictMatch;
		bool	autoindex;
		std::string uri;
		std::set<std::string> allowedMethods;
		std::string root;
		std::string index;
		int clientMaxBodySize;

		bool	isCgi;
		std::string cgiPass;
		std::string cgiIndex;
		std::string cgiInclude;
		std::map<std::string, std::string> cgiParams;

		LocationConfig(Dictionary &dictionary, std::map <std::string, std::vector<std::string> > errorPages, int clientMaxBodySize);
		LocationConfig &operator=(const LocationConfig &rhs);
		~LocationConfig();
		void	setUri(std::vector<std::string> vector);
		void	fillAttributes(std::vector<std::string> confLineVector, Dictionary &dictionary);
		bool	isValid();
		bool	isMethodAllowed(std::string);
};

#endif

#ifndef DICTIONARY_H
# define DICTIONARY_H

# include <string>
# include <map>
# include <set>
# include <vector>
# include <iostream>

class Dictionary
{
	private:

		std::map<std::string, int> configBlockLevels;
		std::set<std::string> configServerAttributes;
		std::set<std::string> configLocationAttributes;

		std::map<std::string, std::string> contentTypes;
		std::set<std::string> methods;
		std::set<std::string> locationsAttributes;
		void _setContentTypes();
		void _setMethods();
		// void _setLocationAttributes();
		void _setConfigDictionary();
	public:
		Dictionary();
		~Dictionary();
		void init();
		bool	isMethodInDictionary(std::string method);
		std::string	getContentTypeFromDictionary(std::string type);
		int getConfigBlockLevel(std::string blockName);
		bool	isAttributeInServerDictionary(std::string attributeName);
		bool	isAttributeInLocationDictionary(std::string attributeName);
};



#endif

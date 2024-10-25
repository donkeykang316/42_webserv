#ifndef DICTIONARY_H
# define DICTIONARY_H

# include <string>
# include <map>
# include <set>
# include <vector>
# include <iostream>

enum eRequestType {
	UNKNOWN_REQUEST_TYPE,
	GET_FILE,
	GET_DIR_LIST,
	CGI,
	POST_DATA,
	DELETE_DATA,
	REDIRECT_REQUEST,
};

class Dictionary
{
	private:

		std::map<std::string, int> configBlockLevels;
		std::set<std::string> configServerAttributes;
		std::set<std::string> configLocationAttributes;

		std::map<std::string, std::string> contentTypes;
		std::set<std::string> methods;
		std::set<std::string> locationsAttributes;

		std::map<std::string, std::string> configVariables;
		std::map<std::string, std::string> supportedCGIExtensions;
		std::map<std::string, std::string> supportedCGIExecutors;

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
		std::string getConfigVariable(std::string alias);
		bool	isAttributeInServerDictionary(std::string attributeName);
		bool	isAttributeInLocationDictionary(std::string attributeName);
		std::string getSupportedCGIExtension(std::string ext);
		std::string getSupportedCGIExecutor(std::string ext);
};



#endif

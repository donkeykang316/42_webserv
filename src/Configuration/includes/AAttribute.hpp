#ifndef AATTRIBUTE_H
# define AATTRIBUTE_H

# include <vector>
# include <map>
# include <string>
# include <iostream>

class Attribute
{
	protected:
		void	addErrorPages(std::vector<std::string> vector);
	public:
		std::map<std::string, std::vector<std::string> > errorPages;
		Attribute();
		virtual ~Attribute()=0;
		bool isValidOneValue(std::vector<std::string> vector);
		bool isValidOneDigitValue(std::vector<std::string> vector);
};

#endif

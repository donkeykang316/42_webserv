#include "includes/AAttribute.hpp"

// Attribute. abstract class for validation of attributes
Attribute::Attribute() {}
Attribute::~Attribute() {}
bool Attribute::isValidOneValue(std::vector<std::string> vector)
{
	if (vector.size() != 2)
	{
		std::cout << "Instruction \"" << vector[0] << "\" must have only one value" << std::endl;
		return (false);
	}
	return (true);
}

bool Attribute::isValidOneDigitValue(std::vector<std::string> vector)
{
	if (!isValidOneValue(vector))
		return (false);
	if (vector[1].find_first_not_of("0123456789") != vector[1].npos)
	{
		std::cout << "Instruction \"" << vector[0] << "\" accept only digits as port number " << vector[1] << std::endl;
		return (false);
	}
	return (true);
}
void Attribute::addErrorPages(std::vector<std::string> vector)
{
	int firstPathIdx = 1;
	int size = vector.size();
	if (size < 4)
	{
		std::cout << "Instruction \"" << vector[0] << "\" requires at least 3 parameters: error_codes path_to_error_html error_html " << std::endl;
		return;
	}
	while (vector[firstPathIdx].find_first_not_of("0123456789") == vector[firstPathIdx].npos)
		firstPathIdx++;
	if (firstPathIdx == 1)
	{
		std::cout << "Instruction \"" << vector[0] << "\" requires as first parameter error_code " << std::endl;
		return;
	}
	if (firstPathIdx + 2 != size)
	{
		std::cout << "Instruction \"" << vector[0] << "\" requires as second and third parameter path_to_error_html and error_html" << std::endl;
		return;
	}
	std::vector<std::string> errPath;
	std::copy(vector.begin() + firstPathIdx, vector.end(), std::back_inserter(errPath));
	for (int i = 1; i < firstPathIdx; i++)
	{
		errorPages[vector[i]] = errPath;
	}
}

#ifndef BITCOIN_EXCHANGE_H
#define BITCOIN_EXCHANGE_H

#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <ctime>

class BitcoinExchange
{
private:
	std::map<time_t, float> _data;
	bool _isDateValid(std::string date, struct tm *tmStruct);
	bool _isFloat(std::string str);
	bool _isCurrencyValid(float value);
	void _pushData(std::string buff);
	void _convertInput(std::string buff);

public:
	BitcoinExchange();
	BitcoinExchange(BitcoinExchange &rhs);
	BitcoinExchange &operator=(BitcoinExchange const &rhs);
	~BitcoinExchange();
	bool init(char *const input_path);
	static std::string const data_file;
};

std::ostream &operator<<(std::ostream &os, const BitcoinExchange &rhs);

#endif

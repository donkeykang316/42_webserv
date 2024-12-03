#ifndef RPN_H
#define RPN_H

#include <stack>
#include <string>
#include <iostream>
#include <stdlib.h>

class RPN
{
private:
	std::stack<std::string> _stack;
	bool _fillStack(std::string str);
	int _sum(int arg1, int arg2);
	int _mult(int arg1, int arg2);
	int _div(int arg1, int arg2);
	int _substr(int arg1, int arg2);
	int _calculate(bool *err);

public:
	RPN();
	RPN(RPN &rhs);
	RPN &operator=(RPN const &rhs);
	RPN(std::string str);
	~RPN();
};

std::ostream &operator<<(std::ostream &os, const RPN &rhs);

#endif

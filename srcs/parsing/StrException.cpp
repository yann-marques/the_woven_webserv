#include "StrException.hpp"

StrException::StrException() {}

StrException::StrException(std::string str): _str(str) {}

StrException::StrException(const StrException& rhs) {
	*this = rhs;
}

StrException&	StrException::operator=(const StrException& rhs) {
	_str = rhs._str;
	return (*this);
}

StrException::~StrException() throw() {}
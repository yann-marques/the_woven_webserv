#ifndef STREXCEPTION_HPP
# define STREXCEPTION_HPP

# include <exception>

class	StrException: public std::exception {
	protected:
		std::string	_str;
	public:
		StrException(std::string str): _str(str) {}
		virtual ~StrException() throw() {}
};

#endif
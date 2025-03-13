#ifndef STREXCEPTION_HPP
# define STREXCEPTION_HPP

# include <exception>
# include <string>

class	StrException: public std::exception {
	protected:
		std::string	_str;
	public:
		StrException();
		StrException(std::string str);
		StrException(const StrException& rhs);
		StrException&	operator=(const StrException& rhs);

		virtual ~StrException() throw();
};

#endif
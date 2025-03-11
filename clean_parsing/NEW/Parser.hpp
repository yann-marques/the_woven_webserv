#ifndef PARSER_HPP
# define PARSER_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <cctype>
# include <map>
# include <vector>
# include <set>
# include <cstddef>
# include <string>
# include <cstdlib>

# include "templates.tpp"
# include "StrException.hpp"
# include "utils.hpp"

class	Parser {
	protected:
		std::set< std::string >	_argsToFind;
		std::multimap< std::string, std::string >	_args;
	public:
		Parser();
		Parser(const Parser& rhs);
		Parser&	operator=(const Parser& rhs);

		virtual void	setArgsToFind() = 0;
		virtual void	checkArgsFormat(const std::multimap< std::string, std::string >& args) const = 0;

		void	checkErrorPages(t_mmap_range< std::string, std::string >::t mmRange) const;
		void	checkCgiPath(t_mmap_range< std::string, std::string >::t range) const;

		size_t	endOfScopeIndex(std::string str, size_t pos);
		std::vector< std::string >	splitScope(std::string fileContent, std::string sep);
		size_t	setArgKey(std::string line, std::string& key);
		size_t	setArgValueLine(std::string line, std::string key, std::string& valueLine, size_t prvPos);
		void	setValues(std::multimap< std::string, std::string >& args, std::string key, std::string valueLine, int scopeCmp);
		std::multimap< std::string, std::string >	parseLine(std::string line);

		void	deleteBrackets(std::vector< std::string >&	vec);

//		virtual void	foo() = 0;

		// templates.tpp

		virtual	~Parser();

		// EXCEPTIONS
		class	ArgOutOfServerScopeException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	ConfigSyntaxException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	UnexpectedKeyException: public StrException {
			public:
				UnexpectedKeyException(std::string where);
				~UnexpectedKeyException() throw();

				const char*	what() const throw();
		};
		class	UnexpectedValueException: public StrException {
			private:
				const std::string	_str;
			public:
				UnexpectedValueException(std::string where);
				~UnexpectedValueException() throw();

				const char*	what() const throw();
		};
		class	DoubleArgException: public StrException {
			public:
				DoubleArgException(std::string where);
				~DoubleArgException() throw();

				const char*	what() const throw();
		};
};

#endif
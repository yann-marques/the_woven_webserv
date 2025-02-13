#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <cstdlib>
# include <iostream>
# include <sstream>
# include <istream>
# include <fstream>
# include <cstring>
# include <vector>
# include <map>
# include <set>

# include "StrException.hpp"
//# include "Rules.hpp"

typedef std::multimap< std::string, std::string >::iterator t_multimap_it;
typedef std::pair< t_multimap_it, t_multimap_it > t_range;

class	Config {
	private:
		std::set< std::string >					_argsToFind;
		std::map< std::string, std::string >	_defaultValues;
	public:
		Config();
		Config(char* fileName);

		void	setArgsToFind();
		void	setDefaultValues();

		std::string	extractFileContent(char* fileName);
		bool	bracketsAreClosed(std::string str);
		size_t	endOfScopeIndex(std::string str, size_t pos);
		std::vector< std::string >	splitLine(std::string fileContent, std::string sep);
		std::multimap< std::string, std::string >	parseLine(std::string line);
		std::multimap< std::string, std::string >	parseServerLine(std::string line);
		void	checkArgsFormat(std::multimap< std::string, std::string >& args);

		~Config();

		// EXCEPTIONS
		class	OpenFileException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	ArgOutsideServerScopeException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	UnclosedScopeException: public std::exception {
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

		class	DoubleArgException: public StrException {
			public:
				DoubleArgException(std::string where);
				~DoubleArgException() throw();

				const char*	what() const throw();
		};

		class	MissingPortException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	MultiplePortsException: public std::exception {
			public:
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
};

#endif
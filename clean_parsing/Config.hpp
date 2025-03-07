#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <cstdlib>
# include <iostream>
# include <sstream>
# include <istream>
# include <fstream>
# include <cstring>
# include <cstddef>
# include <vector>
# include <map>
# include <set>

# include "StrException.hpp"

class Rules;

typedef std::multimap< std::string, std::string >::iterator t_multimap_it;
typedef std::pair< t_multimap_it, t_multimap_it > t_range;

void	printMultimap(std::set< std::string > keys, std::multimap< std::string, std::string > map);

class	Config {
	protected:
		std::set< std::string >					_argsToFind;

		std::set< int >	_ports;
		std::multimap< int, std::string >	_serverNames;
		std::map< int, std::map< std::string, Rules* > >	_parsedConfig;
	public:
		Config();
		Config(const char* fileName);

		Config(const Config& rhs);
		Config&	operator=(const Config& rhs);

		void	setArgsToFind();

		std::string	extractFileContent(const char* fileName);
		bool	bracketsAreClosed(std::string str);
		size_t	endOfScopeIndex(std::string str, size_t pos);
		std::vector< std::string >	splitLine(std::string fileContent, std::string sep);
		std::multimap< std::string, std::string >	parseLine(std::string line);
		std::multimap< std::string, std::string >	parseServerLine(std::string line);
		void	checkPortFormat(size_t count, t_range range);
		void	checkArgsFormat(std::multimap< std::string, std::string >& args);

		// GETTERS

		const std::set< int >&	getPorts() const;
		const std::multimap< int, std::string >&	getServerNames() const;
		const std::map< int, std::map< std::string, Rules* > >&	getParsedConfig() const;

		virtual ~Config();

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

std::ostream&	operator<<(std::ostream& os, const Config& rhs);

#endif
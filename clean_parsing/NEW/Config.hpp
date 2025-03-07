#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "Parser.hpp"

class	Rules;

class	Config: public Parser {
	private:
		std::set< int >	_ports;
		std::multimap< int, std::string >	_serverNames;
		std::map< int, std::map< std::string, Rules* > >	_parsedConfig;
	public:
		Config();
		Config(const Config& rhs);
		Config&	operator=(const Config& rhs);

		Config(const char* fileName);

		std::string	extractFileContent(const char* fileName);
		std::vector< std::string >	splitLine(std::string fileContent, std::string sep);

		~Config();

		// EXCEPTIONS
		class	OpenFileException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	UnclosedScopeException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	BadSpacesException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	ArgOutOfServerScopeException: public std::exception {
			public:
				const char*	what() const throw();
		};
};

std::ostream&	operator<<(std::ostream& os, const Config& rhs);

#endif
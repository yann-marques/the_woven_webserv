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
# include "Rules.hpp"

class	Config {
	private:
		std::vector< std::string >	_vServRawVec;
	//	std::map< int, std::vector<std::string> >	_rawVServConfig; //
		std::map< int, std::map< std::string, Rules* > >	_parsedConfig;
	public:
		Config();
		Config(char* filename);
		Config(const Config& rhs);

		Config&	operator=(const Config& rhs);

		std::string	extractFileContent(char* fileName);
		void	setVServRaw(std::string fileContent);
		void	setArgsToFind(std::set< std::string >& vec);

		size_t	endOfScopeIndex(std::string str, size_t pos);
		void	parseLine(std::set< std::string > argsToFind, std::multimap< std::string, std::string >& argsToSet, std::string line);
		void	parseLocation(std::set< std::string > argsToFind, std::map< std::string, std::multimap< std::string, std::string > >& argToSet, t_range range);
		void	setPort(std::map< std::string, Rules* >& toSet, std::multimap< std::string, std::string > args, std::map< std::string, std::multimap< std::string, std::string > > location);

		void	parseVServRawStr(std::set< std::string > argsToFind, std::multimap< std::string, std::string >& argsToSet, std::string rawStr);
		void	parseVServRawVec();

		std::string	serverSubStr(std::string fileContent, size_t& pos);
		size_t	scopeSubStr(std::string str, size_t pos);
		bool	unclosedScope(std::string str, std::string limiter); //

		void checkArgsFormat(std::map< std::string, std::string > argsToFind, std::multimap< std::string, std::string >& argsToCheck);

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
		class	ConfigSyntaxException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	UnexpectedKeyException: public StrException {
			private:
				const std::string	_str;
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
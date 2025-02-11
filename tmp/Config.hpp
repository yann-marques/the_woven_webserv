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

class	Config {
	private:
		std::vector< std::string >	_vServRawVec;
	//	std::map< int, std::vector<std::string> >	_rawVServConfig; //
	public:
		Config();
		Config(char* filename);
		Config(const Config& rhs);

		Config&	operator=(const Config& rhs);

		std::string	extractFileContent(char* fileName);
		void	setVServRaw(std::string fileContent);
		void	setArgsToFind(std::set< std::string >& vec);

		void	parseVServRawStr(std::set< std::string > argsToFind, std::map< std::string, std::string >& argsToSet, std::string rawStr);
		void	parseVServRawVec();

		std::string	serverSubStr(std::string fileContent, size_t& pos);
		size_t	scopeSubStr(std::string str, size_t pos);
		bool	unclosedScope(std::string str, std::string limiter); //

		void checkArgsFormat(std::map< std::string, std::string > args);

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
		class	UnexpectedKeyException: public std::exception {
			private:
				const std::string	_str;
			public:
				UnexpectedKeyException(std::string where);
				~UnexpectedKeyException() throw(); //////

				const char*	what() const throw();
		};
		class	DoubleArgException: public std::exception {
			private:
				const std::string	_str;
			public:
				DoubleArgException(std::string where);
				~DoubleArgException() throw();
				const char*	what() const throw();
		};
		class	MissingPortException: public std::exception {
			public:
				const char*	what() const throw();
		};
		class	UnexpectedValueException: public std::exception {
			private:
				const std::string	_str;
			public:
				UnexpectedValueException(std::string where);
				~UnexpectedValueException() throw(); //////

				const char*	what() const throw();
		};
};

#endif
#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "Parser.hpp"

class	Rules;

class	Config: public Parser {
	private:
		std::set< std::string >	_hosts;
		std::multimap< std::string, int >	_ports;
		std::map< std::string, std::multimap< int, std::string > >	_serverNames;
		// host -> port -> server_name -> Rules*
	//	std::multimap< std::string, std::map< int, std::map< std::string, Rules* > > >	_parsedConfig;
		std::map< std::string, std::map< int, std::map< std::string, Rules* > > >	_parsedConfig;
		public:
		Config();
		Config(const Config& rhs);
		Config&	operator=(const Config& rhs);

		Config(const char* fileName);

		// INHERITED
		void	setArgsToFind();
		void	checkArgsFormat(const std::multimap< std::string, std::string >& args) const;

		// GETTERS
		const std::set< std::string >&	getHosts() const;
		const std::multimap< std::string, int >&	getPorts() const;
		const std::map< std::string, std::multimap< int, std::string > >&	getServerNames() const;
		const std::map< std::string, std::map< int, std::map< std::string, Rules* > > >&	getParsedConfig() const;

		std::string	extractFileContent(const char* fileName);
		std::vector< std::string >	splitLine(std::string fileContent, std::string sep);

		void	setRules(const std::multimap< std::string, std::multimap< std::string, std::string > >& hostArgs);
		void	setPort(std::string host, int port);
		void	setServerName(const std::string& host, const int& port, const std::string& serverName);
		void	setServerNames(std::string host, int port, t_mmap_range< std::string, std::string >::t argsRange);
		void	setServerNames(const std::multimap< std::string, std::multimap< std::string, std::string > >& hostArgs);
		void	setServerNamesByHost(const t_mmap_range< std::string, std::multimap< std::string, std::string > >::t& range);
		void	setArgsByHost(t_mmap_range< std::string, std::multimap< std::string, std::string > >::t range);
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
		class	MissingPortException: public std::exception {
			public:
				const char*	what() const throw();
		};
};

std::ostream&	operator<<(std::ostream& os, const Config& rhs);

#endif
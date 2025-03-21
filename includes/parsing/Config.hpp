#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "AParser.hpp"

class	Rules;

class	Config: public AParser {
	private:
		std::map< std::string, std::map< int, std::map< std::string, Rules* > > >	_parsedConfig;

	public:
		Config();
		Config(const char* fileName);
		Config(const Config& rhs);
		Config&	operator=(const Config& rhs);

		std::string	extractFileContent(const char* fileName);

		// GETTER
		const std::map< std::string, std::map< int, std::map< std::string, Rules* > > >&	getParsedConfig() const;
		
		// SETTERS
		void	setArgsToFind();
		void	setArgsByHost(t_mmap_range< std::string, std::multimap< std::string, std::string > >::t range);
		void	setRules(const std::multimap< std::string, std::multimap< std::string, std::string > >& hostArgs);

		// CHECKERS
		void	checkPortFormat(t_mmap_range< std::string, std::string >::t mmRange) const;
		void	checkServerNames(t_mmap_range< std::string, std::string >::t mmRange) const;
		void	checkArgsFormat(const std::multimap< std::string, std::string >& args) const;
		
		void	destruct();
		~Config();

		// EXCEPTIONS
		class	IsDirException: public StrException {
			public:
				IsDirException(std::string where);
				~IsDirException() throw();

				const char*	what() const throw();
		};
		class	OpenFileException: public StrException {
			public:
				OpenFileException(std::string where);
				~OpenFileException() throw();

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
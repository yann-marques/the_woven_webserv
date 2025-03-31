/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AParser.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:09:49 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:09:50 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef APARSER_HPP
# define APARSER_HPP

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
# include <unistd.h>
# include <fcntl.h>

# include "templates.tpp"
# include "StrException.hpp"
# include "check_utils.hpp"

class	AParser {
	protected:
		std::set< std::string >	_argsToFind;
		std::multimap< std::string, std::string >	_args;
	public:
		AParser();
		AParser(const AParser& rhs);
		AParser&	operator=(const AParser& rhs);

		// PARSING
		size_t	endOfScopeIndex(std::string str, size_t pos);
		std::vector< std::string >	splitScope(std::string fileContent, std::string sep);
		void	deleteBrackets(std::vector< std::string >&	vec);
		std::multimap< std::string, std::string >	parseLine(std::string line);
		
		// SETTERS
		virtual void	setArgsToFind() = 0;
		size_t	setArgKey(std::string line, std::string& key);
		size_t	setArgValueLine(std::string line, std::string key, std::string& valueLine, size_t prvPos);
		void	setValues(std::multimap< std::string, std::string >& args, std::string key, std::string valueLine, int scopeCmp);
		
		// CHECKERS
		void	checkErrorPages(t_mmap_range< std::string, std::string >::t mmRange) const;
		void	checkCgiPath(t_mmap_range< std::string, std::string >::t range) const;
		void	checkArgNoDouble(t_mmap_range< std::string, std::string >::t range, bool f(std::string)) const;
		void	checkAllowedMethods(t_mmap_range< std::string, std::string >::t mmRange) const;
		virtual void	checkArgsFormat(const std::multimap< std::string, std::string >& args) const = 0;
		void	checkRoot(t_mmap_range< std::string, std::string >::t mmRange) const;
		void	noEmptyStrValues(std::multimap< std::string, std::string > args) const;

		virtual	~AParser();

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
		class	ForbiddenCharException: public StrException {
			public:
				ForbiddenCharException(std::string where);
				~ForbiddenCharException() throw();

				const char*	what() const throw();
		};
		class	EmptyStrException: public std::exception {
			public:
				const char*	what() const throw();
		};
};

#endif
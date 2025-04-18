/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Rules.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:10:06 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:10:07 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RULES_HPP
# define RULES_HPP

# include <iostream>
# include <cstdlib>
# include <vector>
# include <map>
# include <set>
# include <string>
# include <cstddef>

# include "Config.hpp"

typedef std::multimap< std::string, std::string >::iterator t_multimap_it;
typedef std::pair< t_multimap_it, t_multimap_it > t_range;

class	Rules: public AParser {
	private:
		std::string						_locationPath;

		bool							_autoIndex;
		size_t							_maxBodyBytes;
		std::string						_root;
		std::string						_redirect;
		std::string						_upload;

		std::vector< std::string >		_defaultPages;
		std::vector< std::string >		_allowedMethods;

		std::set< int >					_errorKeys;
		std::map< int, std::string >	_errorPages;

		std::set< std::string >			_cgiKeys;
		std::map< std::string, std::string >	_cgiPath;

		std::vector< std::string >			_locationKeys;
		std::map< std::string, Rules* >	_location;
	public:
		Rules();
		Rules(std::multimap< std::string, std::string > args, const Rules& rhs, std::string locationPath);
		Rules(const Rules& rhs);
		Rules&	operator=(const Rules& rhs);
		Rules&	operator|=(const Rules& rhs);
		
		std::multimap< std::string, std::string >	parseLocationLine(std::string line);
		
		// SETTERS
		void	setArgsToFind();
		void	setAutoIndex(t_mmap_range< std::string, std::string >::t range);
		void	setArgs(std::multimap< std::string, std::string > args);
		void	setStrArg(std::string& toSet, t_mmap_range< std::string, std::string >::t range);
		void	setErrorPages(t_range range);
		void	setCgiPath(t_range range);
		void	setLocation(t_range range);
		void	setLocationKey(std::string str);
		void	setVector(std::vector< std::string >& vec, t_range range);
		void	setMaxBodyBytes(t_mmap_range< std::string, std::string >::t range);
		
		
		// CHECKERS
		void	checkArgsFormat(const std::multimap< std::string, std::string >& args) const;

		// GETTERS
		const std::multimap< std::string, std::string >&	getArgs() const;
		const std::string&	getLocationPath() const;
		const std::string&	getRoot() const;
		const std::vector< std::string >&	getDefaultPages() const;
		const std::set< int >&	getErrorKeys() const;
		const std::map< int, std::string >&	getErrorPages() const;
		const bool&	getAutoIndex() const;
		const std::vector< std::string >&	getAllowedMethods() const;
		const size_t&	getMaxBodyBytes() const;
		const std::set< std::string >&	getCgiKeys() const;
		const std::map< std::string, std::string >&	getCgiPath() const;
		const std::string&	getRedirect() const;
		const std::string&	getUpload() const;
		const std::vector< std::string >&	getLocationKeys() const;
		const std::map< std::string, Rules* >&	getLocation() const;

		// RECURSIVE PRINT
		void	printDeep(size_t i, std::string name);

		void	destruct();
		~Rules();

		// EXCEPTIONS
		class	RedefinedArgException: public StrException {
			public:
				RedefinedArgException(std::string where);
				~RedefinedArgException() throw();

				const char*	what() const throw();
		};
		class	InvalidLocationKeyException: public StrException {
			public:
				InvalidLocationKeyException(std::string where);
				~InvalidLocationKeyException() throw();

				const char*	what() const throw();
		};
};

#endif
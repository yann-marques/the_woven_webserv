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

// for each server_name
class	Rules: public Config {
	private:
		std::multimap<std::string, std::string >	_args;
		std::multimap< std::string, std::string >	_inherit;

		std::string						_root;
		std::vector< std::string >		_defaultPages;

		std::set< int >					_errorKeys;
		std::map< int, std::string >	_errorPages; //// no : code d'erreur, fichier

		bool							_autoIndex; // default = true
		std::vector< std::string >		_allowedMethods;
		size_t							_maxBodyBytes;

		std::set< std::string >			_cgiKeys;
		std::map< std::string, std::string >	_cgiPath; // map ?
		std::string						_redirect;
		std::string						_upload;

		std::vector< std::string >			_locationKeys; // ?
		std::map< std::string, Rules* >	_location; // ?
	public:
		Rules();
		Rules(std::multimap< std::string, std::string > args, const Rules& rhs);
//		Rules(std::multimap< std::string, std::string > args, Rules* parent);
		Rules(const Rules& rhs);
		Rules&	operator=(const Rules& rhs);
		Rules&	operator|=(const Rules& rhs);

		void	setAutoIndex(std::string str);
		void	setArgs(std::multimap< std::string, std::string > args);
//		void	setArgs(std::multimap< std::string, std::string > args, Rules* parent);
		void	setInheritArgs(std::multimap< std::string, std::string >& args);
		void	setErrorPages(t_range range);
		void	setCgiPath(t_range range);
		void	setLocation(t_range range);
		void	setLocationKey(std::string str);
		std::multimap< std::string, std::string >	parseLocationLine(std::string line);
		void	setVector(std::vector< std::string >& vec, t_range range);
		size_t	parseMaxBodyBytes(std::string str);

		// GETTERS

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

		void	goDeep(size_t i, std::string name);

		~Rules();

};
// to do
std::ostream&	operator<<(std::ostream& os, const Rules& rhs);
//std::ostream&	operator<<(std::ostream& osm const std::vector< std::string >& vec);

#endif
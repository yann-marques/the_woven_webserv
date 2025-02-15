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
		std::multimap<std::string, std::string >	_rawArgs;

		std::string						_root;
		std::vector< std::string >		_defaultPages;
		std::map< int, std::string >	_errorPages; //// no : code d'erreur, fichier
		bool							_autoIndex; // default = true
		std::vector< std::string >		_allowedMethods;
		size_t							_maxBodyBytes;
		std::vector< std::string >		_cgiPaths; // map ?
		std::string						_redirect;
		std::string						_upload;

		std::vector< std::string >			_locationKeys; // ?
		std::map< std::string, Rules* >	_location; // ?
	public:
		Rules();
		Rules(std::multimap< std::string, std::string > args);
//		Rules(std::multimap< std::string, std::string > args, Rules* parent);
	//	Rules(const Rules& rhs);
	//	Rules&	operator=(const Rules& rhs);

		void	setAutoIndex(std::string str);
		void	setArgs(std::multimap< std::string, std::string > args);
//		void	setArgs(std::multimap< std::string, std::string > args, Rules* parent);
		void	inheritArgs(std::multimap< std::string, std::string >& args);
		void	setLocation(t_range range);
		void	setLocationKey(std::string str);
		std::multimap< std::string, std::string >	parseLocationLine(std::string line);
		void	setVector(std::vector< std::string >& vec, t_range range, std::string key);
		size_t	parseMaxBodyBytes(std::string str);

		void	goDeep(size_t i);

		~Rules();
};
// to do
// std::ostream&	operator<<(std::ostream& os, const Rules& rhs);

#endif
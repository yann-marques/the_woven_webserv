#ifndef RULES_HPP
# define RULES_HPP

# include <iostream>
# include <cstdlib>
# include <vector>
# include <map>
# include <set>
# include <string>

typedef std::multimap< std::string, std::string >::iterator t_multimap_it;
typedef std::pair< t_multimap_it, t_multimap_it > t_range;

// for each server_name
class	Rules {
	private:
		std::string						_root;
		std::vector< std::string >		_defaultPages;
		std::map< int, std::string >	_errorPages; //// no
		bool							_autoIndex; // default = false
		std::vector< std::string >		_allowedMethods;
		size_t							_maxBodyBytes;
		std::vector< std::string >		_cgiPaths; // map ?
		std::string						_redirect;
		std::string						_upload;

		std::map< std::string, Rules* >	_location; // ?
	public:
		Rules();
		Rules(std::multimap< std::string, std::string > args, std::map< std::string, std::multimap< std::string, std::string > > location);
		Rules(std::vector< std::string > _rawStrs);
		Rules(const Rules& rhs);
	//	operator=(const Rules& rhs);

		void	setVector(std::vector< std::string >& vec, t_range range);
		void	setLocation(std::map< std::string, std::multimap< std::string, std::string > > location);

		~Rules();
};

#endif
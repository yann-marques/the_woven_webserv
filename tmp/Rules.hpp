#ifndef RULES_HPP
# define RULES_HPP

# include <vector>
# include <map>
# include <set>
# include <string>

// for each server_name
class	Rules {
	private:
		std::string						_root;
		std::set< std::string >			_defaultPages;
		std::map< int, std::string >	_errorPages; //// no
		bool							_autoIndex; // default = false
		std::set< std::string >			_allowedMethods;
		size_t							_maxBodyBytes;
		std::set< std::string >			_cgiPaths; // map ?
		std::string						_redirect;
		std::string						_upload;

		Rules*							_location;
	public:
		Rules();
		Rules(std::vector< std::string > _rawStrs);
		Rules(const Rules& rhs);
		operator=(const Rules& rhs);
		~Rules();
};

#endif
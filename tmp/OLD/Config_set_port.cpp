#include "Config.hpp"

static void	getServerNames(std::vector< std::string >& toSet, t_range range) {
	t_multimap_it	mit = range.first;
	std::string	str = mit->second;
	do {
		size_t	pos = str.find(',');
		if (pos == std::string::npos)
			pos = str.find(';');
		toSet.push_back(str.substr(0, pos));
		str.erase(0, pos + 1);
	} while (!str.empty());
}

void	Config::setPort(std::map< std::string, Rules* >& toSet, std::multimap< std::string, std::string > args, std::map< std::string, std::multimap< std::string, std::string > > location) {
	(void) toSet;
	(void) location;
	std::vector< std::string >	serverNames;
	getServerNames(serverNames, args.equal_range("server_names"));
	for (size_t i = 0, size = serverNames.size(); i < size; i++) {
	//	std::cout << i << ' ' << serverNames[i] << std::endl;
		// parse location before setting Rules
		
		Rules*	rules = new Rules(args, location); //// hors de la boucle ?
		delete rules;
	}
}
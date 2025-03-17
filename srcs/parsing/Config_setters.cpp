#include "Config.hpp"
#include "Rules.hpp"

void	Config::setArgsToFind() {
	_argsToFind.insert("host");
	_argsToFind.insert("port");
	_argsToFind.insert("server_names");
	
	_argsToFind.insert("location");
	_argsToFind.insert("root");
	_argsToFind.insert("default_pages");
	_argsToFind.insert("error_pages");
	_argsToFind.insert("auto_index");
	_argsToFind.insert("allowed_methods");
	_argsToFind.insert("max_body_bytes");
	_argsToFind.insert("cgi_path");
	_argsToFind.insert("redirect");
	_argsToFind.insert("upload");
}

void	Config::setArgsByHost(t_mmap_range< std::string, std::multimap< std::string, std::string > >::t range) {
	t_mmap_it< std::string, std::multimap< std::string, std::string > >::t
		argsIt = range.first, argsIte = range.second;
	std::string	hostName = argsIt->first;
	std::map< int, std::map< std::string, Rules* > >&
		hostRef = _parsedConfig[hostName];
	Rules	defaultRules;
	while (argsIt != argsIte) {
		std::multimap< std::string, std::string >	args = argsIt->second;
		int	port = std::atoi(args.find("port")->second.c_str());
		std::map< std::string, Rules* >	rulesMap;

		t_mmap_range< std::string, std::string >::t
			serverNamesRange = args.equal_range("server_names");
		t_mmap_it< std::string, std::string >::t
			serverNamesIt = serverNamesRange.first, serverNamesIte = serverNamesRange.second;

		
		while (serverNamesIt != serverNamesIte) {
			std::string serverName = serverNamesIt->second;
			if (!hostRef.count(port) || !hostRef[port].count(serverName))
				hostRef[port][serverName] = new Rules(args, defaultRules, "/");
			serverNamesIt++;
		}
		if (!hostRef[port].count("localhost"))
			hostRef[port]["localhost"] = new Rules(args, defaultRules, "/");
		argsIt++;
	}
}

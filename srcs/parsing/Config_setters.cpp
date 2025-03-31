/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config_setters.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:11:55 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:11:59 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
		std::string	portStr = args.find("port")->second;
		int	port = std::atoi(portStr.c_str());
		
		if (hostRef.count(port))
			throw (MultipleDefinitionOfPort(hostName + ':' + portStr));
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
		if (!hostRef[port].count(hostName))
			hostRef[port][hostName] = new Rules(args, defaultRules, "/");
		if (!hostRef[port].count("localhost"))
			hostRef[port]["localhost"] = new Rules(args, defaultRules, "/");
		argsIt++;
	}
}

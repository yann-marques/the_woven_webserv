#include "Config.hpp"
#include "Rules.hpp"

Config::Config(): Parser() {
	setArgsToFind();
}

Config::Config(const Config& rhs): Parser() {
	setArgsToFind();
	*this = rhs;
}

Config&	Config::operator=(const Config& rhs) {
	(void) rhs;
/*	_ports = rhs.getPorts();
	_serverNames = rhs.getServerNames();
	for (t_set_it< int >::t portIt = _ports.begin(), portIte = _ports.end(); portIt != portIte; portIt++) {
		int	port = *portIt;
		t_mm_range< int, std::string >::t	mmRange = _serverNames.equal_range(port);
		for (t_mm_it< int, std::string >::t mmIt = mmRange.first, mmIte = mmRange.second; mmIt != mmIte; mmIt++) {
			std::string	serverName = mmIt->second;
			_parsedConfig[port][serverName] = new Rules(*(rhs.getParsedConfig().at(port).at(serverName)));
		}
	}
*/	return (*this);
}

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

/*
Config::~Config() {
    t_set_it< std::string >::t
        hostKeyIt = _hosts.begin(), hostKeyIte = _hosts.end();
    while (hostKeyIt != hostKeyIte) {
	//	std::cout << "host: " << *hostKeyIt << " count: "
	//		<< _serverNames.count(*hostKeyIt) << std::endl;
        // KEYS
        t_mmap_range< std::string, int >::t
            portKeyRange = _ports.equal_range(*hostKeyIt);
        t_mmap_it< std::string, int >::t
            portKeyIt = portKeyRange.first, portKeyIte = portKeyRange.second;
        //

        while (portKeyIt != portKeyIte) {
			// KEYS
            t_mmap_range< std::string, std::multimap< int, std::string > >::t
			serverNamesPortRange = _serverNames.equal_range(*hostKeyIt);
            t_mmap_it< std::string, std::multimap< int, std::string > >::t // port // mmap
			serverNamesPortIt = serverNamesPortRange.first,
			serverNamesPortIte = serverNamesPortRange.second;
            //
			
            while (serverNamesPortIt != serverNamesPortIte) {
				std::cout << "\tport: " << portKeyIt->second << " count: "
					<< serverNamesPortIt->second.count(portKeyIt->second) << std::endl;
                // KEYS

                t_mmap_range< int, std::string >::t
                    serverNamesRange = serverNamesPortIt->second.equal_range(portKeyIt->second);
                t_mmap_it< int, std::string >::t
                    serverNamesKeyIt = serverNamesRange.first,
                    serverNamesKeyIte = serverNamesRange.second;
                //
				size_t i = 0;
                while (serverNamesKeyIt != serverNamesKeyIte) {
                    // KEYS
                    std::string serverNameKey = serverNamesKeyIt->second;
                    std::cout  << i << " host: " << *hostKeyIt << "\tport: " << portKeyIt->second << "\tserverNameKey: " << serverNameKey << std::endl;
                    //

                    serverNamesKeyIt++;
					i++;
                }
                serverNamesPortIt++;
            }
            portKeyIt++;
        }
        hostKeyIt++;
    }
}
*/
/*
Config::~Config() {
	t_set_it< std::string >::t	hostIt = _hosts.begin(), hostIte = _hosts.end();
	while (hostIt != hostIte) {
		t_mmap_range< std::string, int >::t	portRange = _ports.equal_range(*hostIt);
		t_mmap_it< std::string, int >::t	portIt = portRange.first, portIte = portRange.second;
		while (portIt != portIte) {
			t_mmap_range< int, std::string >::t	serverNamesRange = _serverNames.equal_range(portIt->second);
			t_mmap_it< int, std::string >::t	serverNamesIt = serverNamesRange.first,
												serverNamesIte = serverNamesRange.second;
			while (serverNamesIt != serverNamesIte) {
				delete _parsedConfig[*hostIt][portIt->second][serverNamesIt->second];
				serverNamesIt++;
			}
			portIt++;
		}
		hostIt++;
	}
}
*/

Config::~Config() {
	t_mmap_it< std::string, std::multimap< int, std::string > >::t
		it = _serverNames.begin(), ite = _serverNames.end();
	while (it != ite) {
		std::string host = it->first;
		t_mmap_it< int, std::string >::t
			portIt = it->second.begin(), portIte = it->second.end();
		while (portIt != portIte) {
			int port = portIt->first;
			std::string serverName = portIt->second;
		//	t_mmap_range< int, std::string >::t
		//		sNamesRange = portIt
		std::cout << "host: " << host << "\tport: " << port << "\tsName: " << serverName << std::endl;
		delete _parsedConfig[host][port][serverName];
			portIt++;
		}
		it++;
	}
}

std::ostream&	operator<<(std::ostream& os, const Config& rhs) {
	(void) rhs;
	std::cout	<< "########################################################## CONFIG" << std::endl << std::endl;

	t_mmap_it< std::string, std::multimap< int, std::string > >::t
		it = rhs.getServerNames().begin(), ite = rhs.getServerNames().end();
	std::string	host = "";
	while (it != ite) {
		if (host != it->first)
			std::cout << "################################################### HOST " << it->first << std::endl << std::endl;
		host = it->first;
		t_mmap_it< int, std::string >::t
			portIt = it->second.begin(), portIte = it->second.end();
		int	port = -1;
		while (portIt != portIte) {
			if (port != portIt->first)
				std::cout << "############################################ PORT " << portIt->first << std::endl << std::endl;
			port = portIt->first;
			std::string serverName = portIt->second;
		//	t_mmap_range< int, std::string >::t
		//		sNamesRange = portIt
		//	std::cout << "host: " << host << "\tport: " << port << "\tsName: " << serverName << std::endl;
			rhs.getParsedConfig().at(host).at(port).at(serverName)->printDeep(0, serverName);
			portIt++;
		}
		it++;
	}
	/*
	t_set_it< std::string >::t	hostKeyIt = rhs.getHosts().begin(), hostKeyIte = rhs.getHosts().end();
	while (hostKeyIt != hostKeyIte) {
		std::cout << "########################################################## HOST: " << *hostKeyIt << std::endl;
		t_mmap_range< std::string, int >::t	portKeyRange = rhs.getPorts().equal_range(*hostKeyIt);
		t_mmap_it< std::string, int >::t
			portKeyIt = portKeyRange.first, portKeyIte = portKeyRange.second;
		while (portKeyIt != portKeyIte) {
			t_mmap_range< int, std::string >::t
				sNameKeyRange = rhs.getServerNames().at(*hostKeyIt).equal_range(portKeyIt->second);
			t_mmap_it< int, std::string >::t
				sNamesKeyIt = sNameKeyRange.first, sNameKeyIte = sNameKeyRange.second;
			while (sNamesKeyIt != sNameKeyIte) {
				rhs.getParsedConfig().at(*hostKeyIt).at(portKeyIt->second).at(sNamesKeyIt->second)->printDeep(0, sNamesKeyIt->second);
				///
				sNamesKeyIt++;
			}
			portKeyIt++;
		}
		hostKeyIt++;
	}
*/
/*
	t_map_it< std::string, std::map< int, std::map< std::string, Rules* > > >::t
		hostIt = rhs.getParsedConfig().begin(), hostIte = rhs.getParsedConfig().end();
	while (hostIt != hostIte) {
		t_map_it< int, std::map< std::string, Rules* > >::t
			portIt = hostIt->second.begin(), portIte = hostIt->second.end();
		while (portIt != portIte) {
			t_map_it< std::string, Rules* >::t
				serverNamesIt = portIt->second.begin(), serverNamesIte = portIte->second.end();
			while (serverNamesIt != serverNamesIte) {
				if (serverNamesIt->second)
					serverNamesIt->second->printDeep(0, serverNamesIt->first);
				serverNamesIt++;
			}
			portIt++;
		}
		hostIt++;
	}
*/
//	t_set_it< std::string >::t	hostIt = rhs.getHosts().begin(), hostIte = 
/*	std::set< int >::iterator	portIt = rhs.getPorts().begin(), portIte = rhs.getPorts().end();
	while (portIt != portIte) {
		std::cout << "########################################################## PORT " << *portIt << std::endl;
		std::multimap< int, std::string >	serverNamesCopy = rhs.getServerNames();
		t_mmap_range< int, std::string >::t	range = serverNamesCopy.equal_range(*portIt);
		t_mmap_it< int, std::string >::t	serverNamesIt = range.first, serverNamesIte = range.second;
		while (serverNamesIt != serverNamesIte) {
			rhs.getParsedConfig().at(*portIt).at(serverNamesIt->second)->printDeep(0, serverNamesIt->second);
			serverNamesIt++;
		}
		portIt++;
	}
*/	return (os);
}

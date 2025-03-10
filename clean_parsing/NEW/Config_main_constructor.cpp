#include "Config.hpp"

std::string	Config::extractFileContent(const char* fileName) {
	std::ifstream	ifs(fileName);
	if (!ifs.is_open())
		throw	OpenFileException();
	
	std::stringstream	buffer;
	buffer << ifs.rdbuf();
	std::string	fileContent = buffer.str();

	ifs.close();

	std::istringstream	iss(fileContent);
	fileContent.clear();

	std::string	word;
	while (iss >> word)
		fileContent += word;

	return (fileContent);
}

static bool	bracketsAreClosed(std::string str) {
	std::string	limiter("{}");

	size_t	count = 0, i = 0;
	while (str[i]) {
		while (str[i] && limiter.find(str[i], 0) == std::string::npos)
			i++;
		if (str[i] == limiter[0])
			count++;
		else if (str[i] == limiter[1])
			count--;
		i++;
	}
	return (count == 0);
}

static bool	badSpaces(std::string str) {
	std::string	limiter("{}:,;");
	for (size_t i = 0; str[i]; i++) {
		if (std::isspace(str[i]) && i > 0
			&& limiter.find(str[i - 1]) == std::string::npos) {
			while (str[i] && std::isspace(str[i]))
				i++;
			if (limiter.find(str[i]) == std::string::npos)
				return (true);
		}
	}
	return (false);
}

void	Config::setPort(std::string host, int port) {
	t_mmap_range< std::string, int >::t	mmRange = _ports.equal_range(host);
	if (!isInRange< std::string, int >(mmRange, port)) {
		_ports.insert(make_pair(host, port));
		std::cout << "insert port: " << port << std::endl;
	}
}
/*
void	Config::setServerNames(std::string host, int port, t_mmap_range< std::string, std::string >::t argsRange) {
//	t_mmap_range< std::string, int >::t	portRange = _ports.equal_range(host);
//	t_mmap_it< std::string, int >::t	portIt = portRange.first, portIte = portRange.second;
	t_mmap_it< std::string, std::string >::t	argsIt = argsRange.first, argsIte = argsRange.second;
//	std::multimap< std::string, std::multimap< int, std::string > >&
	std::multimap< int, std::string >	portMM;

	while (argsIt != argsIte) {
		std::string	serverName = argsIt->second;
		std::pair< int, std::string >	toInsert(port, serverName);

		std::cout << "before insert: " << serverName << std::endl;

		t_mmap_range< std::string, std::multimap< int, std::string > >::t
			hostRange = _serverNames.equal_range(host);
		t_mmap_it< std::string, std::multimap< int, std::string > >::t
			hostIt = hostRange.first, hostIte = hostRange.second;
		if (hostIt == hostIte) {
			portMM.insert(toInsert);
			_serverNames.insert(make_pair(host, portMM));
			std::cout << "insert: " << serverName << std::endl;
		} else {
			while (hostIt != hostIte) {
				portMM = hostIt->second;
				if (!portMM.count(port)
					|| !isInRange< int, std::string >(portMM.equal_range(port), serverName)) {
					portMM.insert(toInsert);
					_serverNames.insert(make_pair(host, portMM));
					std::cout << "insert: " << serverName << std::endl;
				}
				hostIt++;
			}
		}
		portMM.clear();
		argsIt++;
	}
}
*/


/*
		if (!_serverNames.count(host) || hostIt == hostIte) {

			_serverNames.insert(make_pair(host, make_pair(port, serverName)));
		}
		argsIt++;
*/
/*
	if (_serverNames.count(host)) {
		t__mmap_range< std::string, std::multimap< int, std::string > >::t
			hostRange = _serverNames.equal_range(host);
		t_mmap_it< std::string, std::multimap< int, std::string > >::t
			hostIt = hostRange.first, hostIte = hostRange.second;
		while (hostIt != hostIte && !isInRange(hostIt->second.count(port)))
			hostIt++;
		if (hostIt != hostIte) {
			t_mmap_range< int, std::string >::t
				portRange = hostIt->second.equal_range(port);
			t_mmap_it< int, std::string >::t
				portIt = portRange.first, portIt = portRange.second;
			while (argsIt != argsIte) {
				std::string	serverName = argsIt->second;
				if (!isInRange(portRange, serverName))
					_serverNames.insert(make_pair(host, make_pair(port, serverName)))
				argsIt++;
			}
		} else {
			while (argsIt != argsIte) {
				std::string	serverName = argsIt->second;
				if (!hostIt->second.count(port))
					_serverNames.insert(make_pair(host, make_pair(port, serverName)))
				argsIt++;
			}
		}
	}
*/
/*
	if (_serverNames.count(host) && _serverNames.at(host).count(port)
		&& !isInRange(portRange.equal_range(port), argsIt->second))
		_serverNames.insert(make_pair(host, make_pair(port, argsIt->second)));

	std::multimap< int, std::string >	sNames;
	while (argsIt != argsIte) {
		t_mmap_range< std::string, int >::t	portRange = _serverNames.equal_range(host);
		if (isInRange(portRange, port)
			&& !isInRange(portRange.equal_range(port), *argsIt))
		sNames.insert(make_pair(port, *argsIt));
	argsIt++;
	}
	if (!_serverNames.count(host))
		_serverNames.insert(make_pair(host, sNames));
*/

void	Config::setServerName(const std::string& host, const int& port, const std::string& serverName) {
	t_mmap_it< std::string, std::multimap<int, std::string > >::t
		hostIt = _serverNames.begin(), hostIte = _serverNames.end();
		t_mmap_it<int, std::string>::t	portIt, portIte;
	while (hostIt != hostIte) {
		portIt = hostIt->second.begin(), portIte = hostIt->second.end();
		if (hostIt->first == host) {
			while (portIt != portIte) {
				std::cout	<< "host: " << hostIt->first
							<< "\tport: " << portIt->first
							<< "\tserverName: " << portIt->second
							<< std::endl;
				if (portIt->first == port) {
				//	t_mmap_range< int, std::string >::t	portRange = portIt
					if (portIt->second == serverName)
						return ;
				}
				portIt++;
			}
			break ;
		}
		hostIt++;
	}
	std::multimap< int, std::string >	newPorts;
	if (hostIt != hostIte) {
		newPorts = hostIt->second;
		newPorts.insert(make_pair(port, serverName));
		hostIt->second = newPorts;
	}
	newPorts.insert(make_pair(port, serverName));
	_serverNames.insert(make_pair(host, newPorts));
}
/*
	if (hostIt == hostIte) {
		newPorts.insert(make_pair(port, serverName));
		_serverNames.insert(make_pair(host, newPorts));
	} else {
		if (portIt == portIte) {
			newPorts.insert(make_pair(port, serverName));
			_serverNames.insert(make_pair(host, newPorts));
		} else {
			// do nothing
		}
		
	} else {

	}
*/
/*
	if (hostIt != hostIte) {
		if (portIt != portIte && portIt != hostIt->second.end())
			port = portIt->first;
	}
	if (hostIt == hostIte) { //
		newPorts.insert(make_pair(port, serverName));
		_serverNames.insert(make_pair(host, newPorts));
	}
*/


void	Config::setServerNames(std::string host, int port, t_mmap_range< std::string, std::string >::t argsRange) {
	t_mmap_it< std::string, std::string >::t	argsIt = argsRange.first, argsIte = argsRange.second;
	while (argsIt != argsIte) {
		setServerName(host, port, argsIt->second);
		argsIt++;
	}
}
/*
	std::cout << "valeur = " << valeur << std::endl;
	bool valueExists = false;
    for (std::multimap<std::string, std::multimap<int, std::string> >::iterator it = _serverNames.begin(); it != _serverNames.end(); ++it) {
        if (it->first == host) {
            for (std::multimap<int, std::string>::iterator portIt = it->second.begin(); portIt != it->second.end(); ++portIt) {
                if (portIt->first == port && portIt->second == valeur) {
                    valueExists = true;
                    break;
                }
            }
            if (!valueExists) {
                it->second.insert(std::make_pair(port, valeur));
            //    return;
            }
        }
    }

    // Insérer la valeur dans _serverNames si elle n'existe pas déjà
    if (!valueExists) {
        std::multimap<int, std::string> newPorts;
        newPorts.insert(std::make_pair(port, valeur));
        _serverNames.insert(std::make_pair(host, newPorts));
    } else
		valueExists = false;
*/


Config::Config(const char* fileName): Parser() {
	setArgsToFind();

	std::string	fileContent = extractFileContent(fileName);
	if (!bracketsAreClosed(fileContent))
		throw UnclosedScopeException();
	if (badSpaces(fileContent))
		throw BadSpacesException();

	std::vector< std::string >	serverLines = splitScope(fileContent, "server");
	deleteBrackets(serverLines);
//	printVec(serverLines, "\t"); //
	for (size_t i = 0, n = serverLines.size(); i < n; i++) {
		std::multimap< std::string, std::string >	args = parseLine(serverLines[i]);
		checkArgsFormat(args);

		// set host
		std::string	host;
		if (args.count("host"))
			host = args.equal_range("host").first->second;
		else
			host = "127.0.0.1";
		_hosts.insert(host);

		// set port
		int	port = std::atoi(args.equal_range("port").first->second.c_str());
		setPort(host, port);
		printMultimap(_argsToFind, args);
		std::cout << std::endl;
		setServerNames(host, port, args.equal_range("server_names"));
	//	setServerNames(host, port);
		std::cout << "After setServerNames: " << std::endl;
		for (std::multimap<std::string, std::multimap<int, std::string> >::const_iterator it = _serverNames.begin(); it != _serverNames.end(); ++it) {
			std::cout << "Host: " << it->first << std::endl;
			for (std::multimap<int, std::string>::const_iterator portIt = it->second.begin(); portIt != it->second.end(); ++portIt) {
				std::cout << "  Port: " << portIt->first
						  << ", Server Name: " << portIt->second << std::endl;
			}
		}
		std::cout << std::endl;
	//	std::cout << "count port 80: " << _serverNames
	}
	// Set tous les arguments de tous les serveurs apres la boucle.
	// lire tous les serveurs dans la boucle, stocker leurs arguments provisoirement dans:
	// std::vector< std::multimap< std::string, std::string > >
}


#include "Config.hpp"
#include "Rules.hpp"

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

	//	args.erase("host");
	//	args.erase("port");

		t_mmap_range< std::string, std::string >::t
			serverNamesRange = args.equal_range("server_names");
		t_mmap_it< std::string, std::string >::t
			serverNamesIt = serverNamesRange.first, serverNamesIte = serverNamesRange.second;

	//	std::map< int, std::map< std::string, Rules* > >	portMap;
		std::map< std::string, Rules* >	rulesMap;
		
		while (serverNamesIt != serverNamesIte) {
			std::string serverName = serverNamesIt->second;
		//	if (!portMap.count(port) || !rulesMap.count(serverName)) {
				
			Rules*	rules = new Rules(args, defaultRules, "/");
			//	rulesMap[serverName] = rules;
				if (!hostRef.count(port) || !hostRef[port].count(serverName))
					hostRef[port][serverName] = rules;
				else
					delete rules; //
		//	}
			serverNamesIt++;
		}
		if (!hostRef[port].count("localhost")) {
			std::cout << "§§§§§§§§§§§§§§§§§ localhost set" << std::endl;
			hostRef[port]["localhost"] = new Rules(args, defaultRules, "/");
		}
		argsIt++;
	}
}

void	Config::setServerNamesByHost(const t_mmap_range< std::string, std::multimap< std::string, std::string > >::t& range) {
	t_mmap_it< std::string, std::multimap< std::string, std::string > >::t
		argsIt = range.first, argsIte = range.second;
	std::string	host = argsIt->first;
	std::multimap< int, std::string >	portMap;
	while (argsIt != argsIte) {
		std::string	portStr = argsIt->second.find("port")->second;
		int	port = std::atoi(argsIt->second.find("port")->second.c_str());
		t_mmap_range< std::string, std::string >::t
			sNamesRange = argsIt->second.equal_range("server_names");
		t_mmap_it< std::string, std::string >::t
			sNamesIt = sNamesRange.first, sNamesIte = sNamesRange.second;
		while (sNamesIt != sNamesIte) {
			std::string	serverName = sNamesIt->second;
			std::cout << "/////////////// sName = " << serverName << std::endl;
			portMap.insert(std::make_pair(port, serverName));
			sNamesIt++;
		}
		if (!isInRange< int, std::string >(portMap.equal_range(port), "localhost"))
			portMap.insert(std::make_pair(port, "localhost"));
		argsIt++;
	}
	_serverNames.insert(make_pair(host, portMap));
	
}

Config::Config(const char* fileName): Parser() {
	setArgsToFind();

	std::string	fileContent = extractFileContent(fileName);
	if (!bracketsAreClosed(fileContent))
		throw UnclosedScopeException();
	if (badSpaces(fileContent))
		throw BadSpacesException();

	std::vector< std::string >	serverLines = splitScope(fileContent, "server");
	deleteBrackets(serverLines);
	std::multimap< std::string, std::multimap< std::string, std::string > >	hostArgs;
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
		hostArgs.insert(make_pair(host, args));

	//	setServerNames(host, port, args.equal_range("server_names")); ////
	}
	std::cout << "After hostArgs set:" << std::endl;
	for (t_mmap_it< std::string, std::multimap< std::string, std::string > >::t	mmIt = hostArgs.begin(), mmIte = hostArgs.end(); mmIt != mmIte; mmIt++) {
		std::cout << "\thost: " << mmIt->first << "\tcount: " << hostArgs.count(mmIt->first) << std::endl;
		printMultimap(_argsToFind, mmIt->second);
	}
	std::cout << std::endl;
	t_set_it< std::string >::t	hostIt = _hosts.begin(), hostIte = _hosts.end();
	while (hostIt != hostIte) {
		setServerNamesByHost(hostArgs.equal_range(*hostIt));
		setArgsByHost(hostArgs.equal_range(*hostIt));
		hostIt++;
	}
}
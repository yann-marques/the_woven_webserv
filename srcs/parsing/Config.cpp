#include "Config.hpp"
#include "Rules.hpp"

Config::Config(): AParser() {
	setArgsToFind();
}

Config::Config(const char* fileName): AParser() {
	setArgsToFind();

	try {
		std::string	fileContent = extractFileContent(fileName);
		if (!bracketsAreClosed(fileContent))
			throw UnclosedScopeException();

		std::vector< std::string >	serverLines = splitScope(fileContent, "server");
		deleteBrackets(serverLines);
		std::multimap< std::string, std::multimap< std::string, std::string > >	hostArgs;
		for (size_t i = 0, n = serverLines.size(); i < n; i++) {
			std::multimap< std::string, std::string >	args = parseLine(serverLines[i]);
			checkArgsFormat(args);

			std::string	host = setHost(args.equal_range("host"));
			setPort(host, std::atoi(args.equal_range("port").first->second.c_str()));

			hostArgs.insert(make_pair(host, args));
		}
		t_set_it< std::string >::t	hostIt = _hosts.begin(), hostIte = _hosts.end();
		while (hostIt != hostIte) {
			setServerNamesByHost(hostArgs.equal_range(*hostIt));
			setArgsByHost(hostArgs.equal_range(*hostIt));
			hostIt++;
		}
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		destruct();
		throw(e);
	}
}

Config::Config(const Config& rhs): AParser() {
	setArgsToFind();
	*this = rhs;
}

Config&	Config::operator=(const Config& rhs) {
	_hosts = rhs.getHosts();
	_ports = rhs.getPorts();
	_serverNames = rhs.getServerNames();

	t_map_it< std::string, std::map< int, std::map< std::string, Rules* > > >::t
		hostIt = rhs.getParsedConfig().begin(), hostIte = rhs.getParsedConfig().end();
	while (hostIt != hostIte) {
		std::string host = hostIt->first;
		t_map_it< int, std::map< std::string, Rules* > >::t
			portIt = hostIt->second.begin(), portIte = hostIt->second.end();
			while (portIt != portIte) {
				int	port = portIt->first;
				t_map_it< std::string, Rules* >::t
					sNamesIt = portIt->second.begin(), sNamesIte = portIt->second.end();
				while (sNamesIt != sNamesIte) {
					std::string	serverName = sNamesIt->first;
					_parsedConfig[host][port][serverName] = new Rules(*(sNamesIt->second));
					sNamesIt++;
				}
				portIt++;
			}
		hostIt++;
	}

	return (*this);
}

std::string	Config::extractFileContent(const char* fileName) {
	int	fd = open(fileName, O_DIRECTORY);
	if (fd > -1) {
		close(fd);
		throw (IsDirException(fileName));
	}
	std::ifstream	ifs(fileName);
	if (!ifs.is_open())
		throw (OpenFileException(fileName));
	
	std::stringstream	buffer;
	buffer << ifs.rdbuf();
	std::string	fileContent = buffer.str();

	ifs.close();
	if (badSpaces(fileContent))
		throw BadSpacesException();

	std::istringstream	iss(fileContent);
	fileContent.clear();

	std::string	word;
	while (iss >> word)
		fileContent += word;

	return (fileContent);
}

void	Config::destruct() {
	t_mmap_it< std::string, std::multimap< int, std::string > >::t
		it = _serverNames.begin(), ite = _serverNames.end();
	while (it != ite) {
		std::string host = it->first;
		t_mmap_it< int, std::string >::t
			portIt = it->second.begin(), portIte = it->second.end();
		while (portIt != portIte) {
			int port = portIt->first;
			std::string serverName = portIt->second;
			std::map< std::string, Rules* >&
				sNameRef = _parsedConfig[host][port];
			if (sNameRef.count(serverName))
				delete sNameRef[serverName];
			portIt++;
		}
		it++;
	}
}

Config::~Config() {
	destruct();
}

std::ostream&	operator<<(std::ostream& os, const Config& rhs) {
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
			rhs.getParsedConfig().at(host).at(port).at(serverName)->printDeep(0, serverName);
			portIt++;
		}
		it++;
	}
	return (os);
}

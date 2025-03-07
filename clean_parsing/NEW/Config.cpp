#include "Config.hpp"

Config::Config(): Parser() {}

Config::Config(const Config& rhs): Parser() {
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

Config::~Config() {
	t_set_it< int >::t	setIt = _ports.begin(), setIte = _ports.end();
	while (setIt != setIte) { // penser au serveur name par default
		t_mm_range< int, std::string >::t	mmRange = _serverNames.equal_range(*setIt);
		t_mm_it< int, std::string >::t	mmIt = mmRange.first, mmIte = mmRange.second;
		while (mmIt != mmIte) {
		//	delete _parsedConfig[*setIt][mmIt->second];
			mmIt++;
		}
		setIt++;
	}
}

std::ostream&	operator<<(std::ostream& os, const Config& rhs) {
	(void) rhs;
/*	std::cout	<< "########################################################## CONFIG" << std::endl << std::endl;
	std::set< int >::iterator	portIt = rhs.getPorts().begin(), portIte = rhs.getPorts().end();
	while (portIt != portIte) {
		std::cout << "########################################################## PORT " << *portIt << std::endl;
		std::multimap< int, std::string >	serverNamesCopy = rhs.getServerNames();
		t_mm_range< int, std::string >::t	range = serverNamesCopy.equal_range(*portIt);
		t_mm_it< int, std::string >::t	serverNamesIt = range.first, serverNamesIte = range.second;
		while (serverNamesIt != serverNamesIte) {
			rhs.getParsedConfig().at(*portIt).at(serverNamesIt->second)->printDeep(0, serverNamesIt->second);
			serverNamesIt++;
		}
		portIt++;
	}
*/	return (os);
}

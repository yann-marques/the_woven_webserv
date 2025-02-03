#include "Config.hpp"

Config::Config(std::string filename) {
	(void) filename;
	for (int i = 0; i < 5; i++) {
		VServConfig	vsc(i);
		_serverConfig.push_back(vsc);
	}
	_serversNbr = _serverConfig.size();
}

std::size_t		Config::getServersNbr() const {
	return (_serversNbr);
}

VServConfig&	Config::getServerConfig(int i) {
	return (_serverConfig[i]);
}
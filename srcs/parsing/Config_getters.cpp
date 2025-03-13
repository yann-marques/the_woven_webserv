#include "Config.hpp"

const std::set< std::string >&	Config::getHosts() const {
	return (_hosts);
}

const std::multimap< std::string, int >&	Config::getPorts() const {
	return (_ports);
}
const std::map< std::string, std::multimap< int, std::string > >&	Config::getServerNames() const {
	return (_serverNames);
}
const std::map< std::string, std::map< int, std::map< std::string, Rules* > > >&	Config::getParsedConfig() const {
	return (_parsedConfig);
}
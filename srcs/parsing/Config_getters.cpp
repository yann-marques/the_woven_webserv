#include "parsing/Config.hpp"

const std::set< int >&	Config::getPorts() const {
	return (_ports);
}
const std::multimap< int, std::string >&	Config::getServerNames() const {
	return (_serverNames);
}
const std::map< int, std::map< std::string, Rules* > >&	Config::getParsedConfig() const {
	return (_parsedConfig);
}
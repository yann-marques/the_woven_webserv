#include "Config.hpp"

const std::map< std::string, std::map< int, std::map< std::string, Rules* > > >&	Config::getParsedConfig() const {
	return (_parsedConfig);
}
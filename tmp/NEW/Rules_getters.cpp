#include "Rules.hpp"

const std::string&	Rules::getRoot() const {
	return (_root);
}

const std::vector< std::string >&	Rules::getDefaultPages() const {
	return (_defaultPages);
}

const std::set< int >&	Rules::getErrorKeys() const {
	return (_errorKeys);
}

const std::map< int, std::string >&	Rules::getErrorPages() const {
	return (_errorPages);
}

const bool&	Rules::getAutoIndex() const {
	return (_autoIndex);
}

const std::vector< std::string >&	Rules::getAllowedMethods() const {
	return (_allowedMethods);
}

const size_t&	Rules::getMaxBodyBytes() const {
	return (_maxBodyBytes);
}

const std::vector< std::string >&	Rules::getCgiPaths() const {
	return (_cgiPaths);
}

const std::string&	Rules::getRedirect() const {
	return (_redirect);
}

const std::string&	Rules::getUpload() const {
	return (_upload);
}

const std::vector< std::string >&	Rules::getLocationKeys() const {
	return (_locationKeys);
}

const std::map< std::string, Rules* >&	Rules::getLocation() const {
	return (_location);
}

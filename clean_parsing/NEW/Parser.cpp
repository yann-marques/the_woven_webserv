#include "Parser.hpp"

static std::set< std::string >	setArgsToFind() {
	std::set< std::string >	argsToFind;
	argsToFind.insert("location");
	argsToFind.insert("port");
	argsToFind.insert("server_names");
	argsToFind.insert("root");
	argsToFind.insert("default_pages");
	argsToFind.insert("error_pages");
	argsToFind.insert("auto_index");
	argsToFind.insert("allowed_methods");
	argsToFind.insert("max_body_bytes");
	argsToFind.insert("cgi_path");
	argsToFind.insert("redirect");
	argsToFind.insert("upload");
	return (argsToFind);
}

Parser::Parser(): _argsToFind(setArgsToFind()) {}

Parser::Parser(const Parser& rhs): _argsToFind(setArgsToFind()) {
	*this = rhs;
}

Parser&	Parser::operator=(const Parser& rhs) {
	(void) rhs;
	return (*this);
}

size_t	Parser::endOfScopeIndex(std::string str, size_t pos) {
	std::string	brackets("{}");
	pos = str.find(brackets[0], pos);
	if (pos == std::string::npos)
		return (-1);
	size_t	count = 1, i = 1;
	while (str[pos + i] && count) {
		while (str[pos + i] && brackets.find(str[pos + i], 0) == std::string::npos)
			i++;
		if (str[pos + i] == brackets[0])
			count++;
		else if (str[pos + i] == brackets[1])
			count--;
		i++;
	}
	return (pos + i);
}

template< typename T >
void	Parser::printVec(const std::vector< T >& vec, std::string tabs) {
	for (size_t i = 0, n = vec.size(); i < n; i++)
		std::cout << tabs << '\t' << vec[i] << std::endl;
}

template< typename T, typename U >
void	Parser::printMap(std::set< T > keys, const std::map< T, U >& map, std::string tabs) {
	typename t_set_it< T >::t	keyIt = keys.begin(), keyIte = keys.end();
	while (keyIt != keyIte) {
		std::cout << tabs << '\t' << *keyIt << ": " << map.at(*keyIt) << std::endl;
		keyIt++;
	}
}

Parser::~Parser() {}
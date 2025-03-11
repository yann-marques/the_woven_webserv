#include "Parser.hpp"

Parser::Parser() {}

Parser::Parser(const Parser& rhs) {
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

std::vector< std::string >	Parser::splitScope(std::string fileContent, std::string sep) {
	std::vector< std::string >	vec;
	size_t	pos = 0, end = 0;

	while (!fileContent.empty()) {
		pos = fileContent.find(sep, 0);
		if (pos == std::string::npos)
			break ;
		end = endOfScopeIndex(fileContent, pos);
		vec.push_back(fileContent.substr(pos + sep.size(), end - sep.size()));
		fileContent.erase(pos, end - pos);
	}
	if (!fileContent.empty())
		throw (ArgOutOfServerScopeException()); /////
	return (vec);
}

size_t	Parser::setArgKey(std::string line, std::string& key) {
	size_t	pos = line.find('{');

	if (line.compare(0, pos, "error_pages") && line.compare(0, pos, "cgi_path"))
		pos = line.find(':');
	if (pos == std::string::npos)
		throw ConfigSyntaxException();
	key = line.substr(0, pos);
//	std::cout << "key: " << key << std::endl;
	return (pos);
}

size_t	Parser::setArgValueLine(std::string line, std::string key, std::string& valueLine, size_t prvPos) {
	size_t	pos;
	t_set_it< std::string >::t	setIt = _argsToFind.find(key);

	if (setIt == _argsToFind.end()) {
	//	std::cout << "here" << std::endl;
		throw UnexpectedKeyException(key);
	}
	else if (*setIt == "location" || *setIt == "error_pages" || *setIt == "cgi_path")
		pos = endOfScopeIndex(line, line.find('{')) - 1;
	else
		pos = line.find(';');

	if (pos == std::string::npos)
		throw (ConfigSyntaxException());

	valueLine = line.substr(prvPos + 1, pos - prvPos);

	return (pos);
}

void	Parser::setValues(std::multimap< std::string, std::string >& args, std::string key, std::string valueLine, int scopeCmp) {
	while (!valueLine.empty()) {
		size_t	pos;
		if (!scopeCmp) {
			pos = valueLine.find(';');
			if (pos == std::string::npos)
				throw (ConfigSyntaxException());
		} else {
			pos = valueLine.find(',');
			if (pos == std::string::npos)
				pos = valueLine.find(';');
		}
		args.insert(make_pair(key, valueLine.substr(0, pos)));
		valueLine.erase(0, pos + 1);
	}
}

std::multimap< std::string, std::string >	Parser::parseLine(std::string line) {
	std::multimap< std::string, std::string >	args;
	std::string	key, valueLine;

	do {
		size_t	pos1 = setArgKey(line, key),
				pos2 = setArgValueLine(line, key, valueLine, pos1);

		if (!key.compare("location"))
			args.insert(make_pair(key, valueLine));
		else {
			int	scopeCmp = key.compare("error_pages") && key.compare("cgi_path");
			if (!scopeCmp) {
				if (line[key.size()] != '{')
					throw (ConfigSyntaxException());
				valueLine.erase(valueLine.size() - 1);
			}
			setValues(args, key, valueLine, scopeCmp);
		}
		line.erase(0, pos2 + 1);
	} while (!line.empty());

	return (args);
}

void	Parser::deleteBrackets(std::vector< std::string >&	vec) {
	for (size_t i = 0, n = vec.size(); i < n; i++) {
		vec[i].erase(0, 1);
		vec[i].erase(vec[i].size() - 1, 1);
	}
}

void	Parser::checkErrorPages(t_mmap_range< std::string, std::string >::t mmRange) const {
	t_mmap_it< std::string, std::string >::t	mmIt = mmRange.first, mmIte = mmRange.second;
	do {
		std::string	str = mmIt->second;
		size_t	pos1 = str.find(':');
		if (pos1 == std::string::npos)
			throw(ConfigSyntaxException());
		std::string	key(str.substr(0, pos1)), value(str.substr(pos1 + 1));
		if (!isDigitStr(key)) // + verification du format de value ?
			throw (UnexpectedKeyException(key));
		mmIt++;
	} while (mmIt != mmIte);
}

void	Parser::checkCgiPath(t_mmap_range< std::string, std::string >::t range) const {
	t_mmap_it< std::string, std::string >::t	mmIt = range.first, mmIte = range.second;
	do {
		std::string	str = mmIt->second;
		size_t	pos1 = str.find(':');
		if (pos1 == std::string::npos) {
			throw(ConfigSyntaxException());
		}
		std::string	key(str.substr(0, pos1)), value(str.substr(pos1 + 1));
		if (key[0] != '.') // + verification du format de value ?
			throw (UnexpectedKeyException(key));
		mmIt++;
	} while (mmIt != mmIte);
}

Parser::~Parser() {}
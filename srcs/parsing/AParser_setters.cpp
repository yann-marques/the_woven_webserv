#include "AParser.hpp"

size_t	AParser::setArgKey(std::string line, std::string& key) {
	size_t	pos = line.find('{');

	if (line.compare(0, pos, "error_pages") && line.compare(0, pos, "cgi_path"))
		pos = line.find(':');
	if (pos == std::string::npos)
		throw ConfigSyntaxException();
	key = line.substr(0, pos);
	return (pos);
}

size_t	AParser::setArgValueLine(std::string line, std::string key, std::string& valueLine, size_t prvPos) {
	size_t	pos;
	t_set_it< std::string >::t	setIt = _argsToFind.find(key);

	if (setIt == _argsToFind.end())
		throw UnexpectedKeyException(key);
	else if (*setIt == "location" || *setIt == "error_pages" || *setIt == "cgi_path")
		pos = endOfScopeIndex(line, line.find('{')) - 1;
	else
		pos = line.find(';');

	if (pos == std::string::npos)
		throw (ConfigSyntaxException());

	valueLine = line.substr(prvPos + 1, pos - prvPos);

	return (pos);
}

void	AParser::setValues(std::multimap< std::string, std::string >& args, std::string key, std::string valueLine, int scopeCmp) {
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
		if (key == "cgi_path") {
			std::cout << std::endl << "parseLine" << std::endl
				<< "key = " << key << std::endl
				<< "valueLine = " << valueLine << std::endl;
		}
		valueLine.erase(0, pos + 1);
	}
}
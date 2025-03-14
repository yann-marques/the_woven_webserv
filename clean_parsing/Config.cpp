#include "Config.hpp"
#include "Rules.hpp"

static bool	isInRange(std::string str, std::pair< std::multimap< int, std::string >::iterator, std::multimap< int, std::string >::iterator > range) {
	std::multimap< int, std::string >::iterator	mmIt = range.first, mmIte = range.second;
	while (mmIt != mmIte && mmIt->second != str)
		mmIt++;
	return (mmIt != mmIte);
}

Config::Config(const char* fileName) {
	setArgsToFind();

	std::string	fileContent = extractFileContent(fileName);
	if (!bracketsAreClosed(fileContent)) // check spaces
		throw UnclosedScopeException();

	std::vector< std::string >	serverLines = splitLine(fileContent, "server");
	for (size_t i = 0, n = serverLines.size(); i < n; i++) {
		std::multimap< std::string, std::string >	args = parseServerLine(serverLines[i]);
		checkPortFormat(args.count("port"), args.equal_range("port"));
		if (!args.count("server_names"))
			args.insert(std::make_pair("server_names", "localhost"));
		checkArgsFormat(args);

		int	port = std::atoi(args.equal_range("port").first->second.c_str());
		args.erase("port");
		_ports.insert(port);

		t_range	range = args.equal_range("server_names");
		t_multimap_it	mit = range.first, mite = range.second;
		std::map< std::string, Rules* >	toSet;
		while (mit != mite) {
			if (!isInRange(mit->second, _serverNames.equal_range(port))) //
				_serverNames.insert(make_pair(port, mit->second));
			mit++;
		}
		if (!isInRange("localhost", _serverNames.equal_range(port)))
			_serverNames.insert(std::make_pair(port, "localhost"));

		args.erase("server_names");

		std::pair< std::multimap< int, std::string >::iterator, std::multimap< int, std::string >::iterator >	serverNamesRange = _serverNames.equal_range(port);
		std::multimap< int, std::string >::iterator	mmIt = serverNamesRange.first, mmIte = serverNamesRange.second;
		while (mmIt != mmIte) {
			if (!isInRange(mit->second, _serverNames.equal_range(port))) {
				Rules	defaultRules;
				Rules*	rules = new Rules(args, defaultRules, "/");
				//rules->printDeep(0, mmIt->second); ///////
				toSet[mmIt->second] = rules;
				if (!_parsedConfig.count(port))
					_parsedConfig[port] = toSet;
				else if (!_parsedConfig[port].count(mmIt->second))
					_parsedConfig[port][mmIt->second] = toSet[mmIt->second];
				else
					delete rules;
			}
			mmIt++;
		}
	} 
	// catch in Webserv() ?
}


std::multimap< std::string, std::string >	Config::parseLine(std::string line) {
	std::multimap< std::string, std::string >	args;
	do {
		size_t	sepPos1 = line.find('{'), sepPos2;
		if (line.compare(0, sepPos1, "error_pages") && line.compare(0, sepPos1, "cgi_path"))
			sepPos1 = line.find(':');
		
		if (sepPos1 == std::string::npos)
			throw ConfigSyntaxException();
		std::string	key(line.substr(0, sepPos1));

		std::set< std::string >::iterator	it = _argsToFind.find(key), ite = _argsToFind.end();
		if (it == ite)
			throw UnexpectedKeyException(key);
		else if (*it == "location" || *it == "error_pages" || *it == "cgi_path")
			sepPos2 = endOfScopeIndex(line, line.find('{')) - 1;
		else
			sepPos2 = line.find(';');

		if (sepPos2 == std::string::npos)
			throw ConfigSyntaxException();

		std::string	value(line.substr(sepPos1 + 1, sepPos2 - sepPos1));

		if (!key.compare("location"))
			args.insert(make_pair(key, value));
		else {
			int	errCmp = key.compare("error_pages");
			if (errCmp)
				errCmp = key.compare("cgi_path");
			if (!errCmp) {
				if (line[key.size()] != '{')
					throw (ConfigSyntaxException());
				value.erase(value.size() - 1);
			}
			while (!value.empty()) {
				size_t	pos;
				if (!errCmp) {
					pos = value.find(';');
					if (pos == std::string::npos)
						throw (ConfigSyntaxException());
				} else {
					pos = value.find(',');
					if (pos == std::string::npos)
						pos = value.find(';');
				}
				std::string	valueSubstr = value.substr(0, pos);
				args.insert(make_pair(key, valueSubstr));
				value.erase(0, pos + 1);
			}
		}
		line.erase(0, sepPos2 + 1);
	} while (!line.empty());
	return (args);
}

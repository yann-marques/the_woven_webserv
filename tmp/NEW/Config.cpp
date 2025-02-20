#include "Config.hpp"
#include "Rules.hpp"

Config::Config() {
	setArgsToFind();
	setDefaultValues();
}

Config::Config(char* fileName) {
	try {
		setArgsToFind();
		setDefaultValues();

		std::string	fileContent = extractFileContent(fileName);
		if (!bracketsAreClosed(fileContent))
			throw UnclosedScopeException();

		std::vector< std::string >	serverLines = splitLine(fileContent, "server");
		for (size_t i = 0, n = serverLines.size(); i < n; i++) {
			std::multimap< std::string, std::string >	args = parseServerLine(serverLines[i]);
			checkArgsFormat(args);
			int	port = std::atoi(args.equal_range("port").first->second.c_str());
			if (!_ports.count(port)) { // pas de port en double. le premier serveur defini avec un port est le seul ? // non
				_ports.insert(port);
				t_range	range = args.equal_range("server_names");
				t_multimap_it	mit = range.first, mite = range.second;
				std::map< std::string, Rules* >	toSet;
				while (mit != mite) {
					Rules	defaultRules;
					Rules*	rules = new Rules(args, defaultRules);
					_serverNames.insert(make_pair(port, mit->second));
					toSet[mit->second] = rules;
			//		toSet[mit->second]->goDeep(0, mit->second);
					mit++;
				}
				_parsedConfig[port] = toSet;
			}
		}
	} catch (OpenFileException& e) {
		std::cerr << e.what() << fileName << std::endl;
	} catch (ArgOutsideServerScopeException& e) {
		std::cerr << e.what() << std::endl;
	} catch (UnclosedScopeException& e) {
		std::cerr << e.what() << std::endl;
	} catch (ConfigSyntaxException& e) {
		std::cerr << e.what() << std::endl;
	} catch (UnexpectedKeyException& e) {
		std::cerr << e.what() << std::endl;
	} catch (DoubleArgException& e) {
		std::cerr << e.what() << std::endl;
	} catch (MissingPortException& e) {
		std::cerr << e.what() << std::endl;
	} catch (MultiplePortsException& e) {
		std::cerr << e.what() << std::endl;
	} catch (UnexpectedValueException& e) {
		std::cerr << e.what() << std::endl;
	}
	// catch in Webserv() ?
}

void	Config::setArgsToFind() {
//	_argsToFind.insert("server");
	_argsToFind.insert("location");
	_argsToFind.insert("port");
	_argsToFind.insert("server_names");
	_argsToFind.insert("root");
	_argsToFind.insert("default_pages");
	_argsToFind.insert("error_pages");
	_argsToFind.insert("auto_index");
	_argsToFind.insert("allowed_methods");
	_argsToFind.insert("max_body_bytes");
	_argsToFind.insert("cgi_path");
	_argsToFind.insert("redirect");
	_argsToFind.insert("upload");
}

void	Config::setDefaultValues() {
	_defaultValues.insert(std::make_pair("location", "NONE"));
	_defaultValues.insert(std::make_pair("port", "80")); // obligatoire
	_defaultValues.insert(std::make_pair("server_names", "NULL"));
	_defaultValues.insert(std::make_pair("root", "www/default.html")); // obligatoire ?
	_defaultValues.insert(std::make_pair("default_pages", "NULL")); //

	_defaultValues.insert(std::make_pair("error_pages", "NULL")); //

	_defaultValues.insert(std::make_pair("auto_index", "true"));

	_defaultValues.insert(std::make_pair("allowed_methods", "GET"));
	_defaultValues.insert(std::make_pair("allowed_methods", "POST"));
	_defaultValues.insert(std::make_pair("allowed_methods", "DELETE"));

	_defaultValues.insert(std::make_pair("max_body_bytes", "1024"));
	_defaultValues.insert(std::make_pair("cgi_path", "NULL")); //
	_defaultValues.insert(std::make_pair("redirect", "NULL")); //
	_defaultValues.insert(std::make_pair("upload", "NULL")); //
}

std::string	Config::extractFileContent(char* fileName) {
	std::ifstream	ifs(fileName);
	if (!ifs.is_open())
		throw	OpenFileException();
	
	std::stringstream	buffer;
	buffer << ifs.rdbuf();
	std::string	fileContent = buffer.str();

	ifs.close();

	std::istringstream	iss(fileContent);
	fileContent.clear();

	std::string	word;
	while (iss >> word)
		fileContent += word;

	return (fileContent);
}

bool	Config::bracketsAreClosed(std::string str) {
	std::string	limiter("{}");

	size_t	count = 0, i = 0;
	while (str[i]) {
		while (str[i] && limiter.find(str[i], 0) == std::string::npos)
			i++;
		if (str[i] == limiter[0])
			count++;
		else if (str[i] == limiter[1])
			count--;
		i++;
	}
	return (count == 0);
}

size_t	Config::endOfScopeIndex(std::string str, size_t pos) {
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

std::vector< std::string >	Config::splitLine(std::string fileContent, std::string sep) {
	std::vector< std::string >	vec;
	size_t	pos = 0, end = 0;

	while (!fileContent.empty()) {
		pos = fileContent.find(sep, 0);
		if (pos == std::string::npos)
			break ;
		end = endOfScopeIndex(fileContent, 0);
		vec.push_back(fileContent.substr(pos, end));
		fileContent.erase(pos, end - pos);
	}
	if (!fileContent.empty()) {
		throw (ArgOutsideServerScopeException());
	}
	return (vec);
}

std::multimap< std::string, std::string >	Config::parseLine(std::string line) {
	std::multimap< std::string, std::string >	args;
	do {
		size_t	sepPos1 = line.find('{'), sepPos2;
		if (line.compare(0, sepPos1, "error_pages") && line.compare(0, sepPos1, "cgi_path"))
			sepPos1 = line.find(':');
		if (sepPos1 == std::string::npos) {
	//		std::cout << "HERE" << std::endl;
			throw ConfigSyntaxException();
		}
		std::string	key(line.substr(0, sepPos1));

		std::set< std::string >::iterator	it = _argsToFind.find(key), ite = _argsToFind.end();
		if (it == ite)
			throw UnexpectedKeyException(key);
		else if (*it == "location" || *it == "error_pages" || *it == "cgi_path")
			sepPos2 = endOfScopeIndex(line, line.find('{')) - 1;
		else
			sepPos2 = line.find(';');

		if (sepPos2 == std::string::npos) {
		//	std::cout << "HERE" << std::endl;
			throw ConfigSyntaxException();
		}

		std::string	value(line.substr(sepPos1 + 1, sepPos2 - sepPos1));
//		std::cout << "VALUE: " << value << std::endl;
		if (!key.compare("location"))
			args.insert(make_pair(key, value));
/*		else if (!key.compare("error_pages")) {
			value.erase(value.size() - 1);
			while (!value.empty()) {
				size_t	pos = value.find(';');
				if (pos == std::string::npos)
					throw (ConfigSyntaxException());
				std::string	errorPagesValue = value.substr(0, pos + 1);
				args.insert(make_pair(key, errorPagesValue));
				value.erase(0, pos + 1);
			}
*/		else {
	//		std::cout << "value: " << value << std::endl;
			int	errCmp = key.compare("error_pages");
			if (errCmp)
				errCmp = key.compare("cgi_path"); /////
			if (!errCmp)
				value.erase(value.size() - 1);
			while (!value.empty()) {
				size_t	pos;
				if (!errCmp) {
					pos = value.find(';');
					if (pos == std::string::npos) {
				//		std::cerr << "HERE ; value: " << value << std::endl;
						throw (ConfigSyntaxException());
					}
				//	pos++;
				} else {
					pos = value.find(',');
					if (pos == std::string::npos)
						pos = value.find(';');
				}
				std::string	valueSubstr = value.substr(0, pos);
				args.insert(make_pair(key, valueSubstr));
				value.erase(0, pos + 1);			
	//			std::cout << key << " : " << valueSubstr << std::endl;
			}
		}

/*
		if (!key.compare("error_pages")) {
			value.erase(value.size() - 1);
			while (!value.empty()) {
				size_t	pos = value.find(';');
				if (pos == std::string::npos) {
//					std::cout << "HERE / value: " << value << std::endl;
					throw (ConfigSyntaxException());
				}
				std::string	errorPagesValue = value.substr(0, pos + 1);
	//			std::cout << "errorPages: " << key << " : " << errorPagesValue << " // " << value << std::endl;
				args.insert(make_pair(key, errorPagesValue));
				value.erase(0, pos + 1);
			}

		} else {
//			std::cout << "///// " << key << " : " << value << std::endl;
			args.insert(make_pair(key, value));
		}
*/			line.erase(0, sepPos2 + 1);
	} while (!line.empty());
	return (args);
}

std::multimap< std::string, std::string >	Config::parseServerLine(std::string line) {
	size_t	pos = line.find("server", 0);
	size_t	argLen = 6;
	if (!line.compare(pos, argLen, "server")) {
		line.erase(pos, argLen + 1);
		pos = line.rfind('}');
		line.erase(pos, 1);
	}
	return (parseLine(line));
}

Config::~Config() {
	std::set< int >::iterator	it = _ports.begin(), ite = _ports.end();
	while (it != ite) { // penser au serveur name par default
		std::pair< std::multimap< int, std::string >::iterator, std::multimap< int, std::string >::iterator >	range = _serverNames.equal_range(*it);
		std::multimap< int, std::string >::iterator	mit = range.first, mite = range.second;
		while (mit != mite) {
			delete _parsedConfig[*it][mit->second];
			mit++;
		}
		it++;
	}
}

std::ostream&	operator<<(std::ostream& os, const Config& rhs) {
	std::cout	<< "//////////////////// CONFIG" << std::endl << std::endl;
	std::set< int >::iterator	portIt = rhs.getPorts().begin(), portIte = rhs.getPorts().end();
	while (portIt != portIte) {
		std::cout << "/////////////// PORT " << *portIt << std::endl;
		std::multimap< int, std::string >	serverNamesCopy = rhs.getServerNames();
		std::pair< std::multimap< int, std::string >::iterator,
				std::multimap< int, std::string >::iterator >	range = serverNamesCopy.equal_range(*portIt);
		std::multimap< int, std::string >::iterator	serverNamesIt = range.first, serverNamesIte = range.second;
		while (serverNamesIt != serverNamesIte) {
			std::cout << *(rhs.getParsedConfig().at(*portIt).at(serverNamesIt->second)) << std::endl;
			serverNamesIt++;
		}
		portIt++;
	}
	return (os);
}

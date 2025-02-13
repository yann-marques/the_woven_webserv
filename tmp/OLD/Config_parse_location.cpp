#include "Config.hpp"

void	Config::parseLine(std::set< std::string > argsToFind, std::multimap< std::string, std::string >& argsToSet, std::string line) {
	do {
		size_t	sepPos1 = line.find(':'), sepPos2;
		if (sepPos1 == std::string::npos)
			throw ConfigSyntaxException(); // details ?
		std::string	key(line.substr(0, sepPos1));

		// check if key is in reserved words
		std::set< std::string >::iterator	it = argsToFind.find(key), ite = argsToFind.end();
		if (it == ite)
			throw UnexpectedKeyException(key); // unexpected key
		else if (*it == "location") {
			sepPos2 = endOfScopeIndex(line, line.find('{')) - 1;
		} else {
			sepPos2 = line.find(';');
		}

		if (sepPos2 == std::string::npos)
			throw ConfigSyntaxException(); // details ?
		std::string	value(line.substr(sepPos1 + 1, sepPos2 - sepPos1));

		std::cout
//			<< "line = |" << line << "|" << std::endl
			<< "key = |" << key << "|" << std::endl
			<< "value = |" << value << "|" << std::endl;

// Quels blocs de parametres peuvent etre en double ?
		if (key != "location" && key != "error_pages" && argsToSet.count(key))
			throw DoubleArgException(key); // location, error_pages peut etre en double. multimap ?
		argsToSet.insert(make_pair(key, value)); ////
		line.erase(0, sepPos2 + 1);
//		std::cout << "line apres erase = |" << line << "|" << std::endl; 
	} while (!line.empty()); //
}

// first location parsing
// if nested locations: parsing later in Rules
void	Config::parseLocation(std::set< std::string > argsToFind, std::map< std::string, std::multimap< std::string, std::string > >& argsToSet, t_range range) {
	t_multimap_it	mit = range.first, mite = range.second;

	while (mit != mite) {
		std::string	line = mit->second;
		std::string	loc("location");
		size_t	argLen = loc.size(), pos = 0;
		if (!line.compare(pos, argLen, loc)) {
			line.erase(pos, argLen + 1);
			pos = line.rfind('}');
			line.erase(pos, 1);
		}
		pos = line.find('{');
		std::string	key = line.substr(0, pos);
		line.erase(0, pos + 1);
		pos = line.rfind('}');
		line.erase(pos, 1);
//		std::cout << "newLoc = |" << line << "|\nkey = " << key << std::endl;
		parseLine(argsToFind, argsToSet[key], line);
		mit++;
	}
}

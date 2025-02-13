#include "Config.hpp"

Config::Config(char* fileName) {
	try {
		std::string	fileContent = extractFileContent(fileName);
		if (!unclosedScope(fileContent, "{}"))
			throw UnclosedScopeException();
		//	std::cout << "\tstr" << std::endl << fileContent << std::endl; //
		setVServRaw(fileContent);
		parseVServRawVec();
	} catch (OpenFileException& e) {
		std::cerr << e.what() << fileName << std::endl;
	} catch (UnclosedScopeException& e) {
		std::cerr << e.what() << std::endl;
	} catch (ConfigSyntaxException& e) {
		std::cerr << e.what() << std::endl;
	} catch (UnexpectedKeyException& e) { // to complete ?
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

std::string	Config::extractFileContent(char* fileName) {
	std::ifstream	ifs(fileName);
	if (!ifs.is_open())
		throw	OpenFileException();
	
	std::stringstream	buffer;
	buffer << ifs.rdbuf();
	std::string	str = buffer.str();

	ifs.close();
	return (str);
}

void	Config::setVServRaw(std::string fileContent) {
	for (size_t pos = 0, end = 0, i = 0; fileContent[pos]; i++) {
		std::string	tmp = serverSubStr(fileContent, end);
//		std::cout << "\tTMP\n|" << tmp << '|' << std::endl;
		if (tmp.empty())
			break ;
		_vServRawVec.push_back(tmp);
//		_vServRawVec.push_back(serverSubStr(fileContent, end));
		end += _vServRawVec[i].size();
		if (fileContent[end])
		end++;
		pos = end;
	}
/*
	std::cout << "vector size = " << _vServRaw.size() << std::endl;
	for (size_t i = 0, size = _vServRaw.size(); i < size; i++)
		std::cout << "\t" << i << std::endl << _vServRaw[i] << std::endl;
*/
}

void	Config::setArgsToFind(std::set< std::string >& argsToFind) {
//	argsToFind.insert("server");
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
}

size_t	Config::endOfScopeIndex(std::string str, size_t pos) {
	std::string	brackets("{}");
	pos = str.find(brackets[0], pos);
	if (pos == std::string::npos)
		return (0);
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
//	std::cout << "scope: " << str.substr(pos, i) << std::endl;
	return (pos + i);
}

void	Config::parseVServRawStr(std::set< std::string > argsToFind, std::multimap< std::string, std::string >& argsToSet, std::string rawStr) {
	std::istringstream	iss(rawStr);
	std::string	word, newStr;

	while (iss >> word)
		newStr += word;
//	std::cout << "newStr = |" << newStr << "|" << std::endl;

	// erase server{}
	size_t	pos = newStr.find("server", 0);
	if (pos != std::string::npos) {
		size_t	argLen = 6;
		if (!newStr.compare(pos, argLen, "server")) {
			newStr.erase(pos, argLen + 1);
			pos = newStr.rfind('}');
			newStr.erase(pos, 1);
		}
	}
	// extract args
	parseLine(argsToFind, argsToSet, newStr);
}

void	Config::parseVServRawVec() {
//	map< int fd, map< string server_name, Rules* >
//	std::map< int, std::map< std::string, Rules* >

	std::set< std::string >	argsToFind;
	setArgsToFind(argsToFind);

	std::multimap< std::string, std::string >	argsToSet;

	for (size_t i = 0, rawSize = _vServRawVec.size(); i < rawSize; i++) {
	//	for (size_t j = 0, argSize = argsToFind.size(); i++)
		parseVServRawStr(argsToFind, argsToSet, _vServRawVec[i]); //
		// check args
		checkArgsFormat(argsToFind, argsToSet); // CHECKPOINT

		// get port
		int port = std::atoi(argsToSet.find("port")->second.c_str());
	//	std::cout << "PORT: " << port << std::endl;
		// parse location // later
		// keys a part. location = "simple" map
		std::map< std::string, std::multimap< std::string, std::string > >	location;
		if (argsToSet.count("location")) {
			parseLocation(argsToFind, location, argsToSet.equal_range("location"));
			argsToSet.erase("location"); ///////
		}
		// checker location.size() dans setPort
		setPort(_parsedConfig[port], argsToSet, location);
		// get server_names
		// separate the rest
	//	argsToSet.push_back();
		argsToSet.clear();
		location.clear();
	//	break ; // test
	}
}

std::string	Config::serverSubStr(std::string fileContent, size_t& pos) {
	std::string	toFind("server");
	pos = fileContent.find(toFind, pos);
	if (pos == std::string::npos)
		return ("");

	std::string	whiteSpaces("\t\n\v\f\r ");
	size_t	pos2 = pos + toFind.size();
	while (fileContent[pos2] && whiteSpaces.find(fileContent[pos2]) != std::string::npos)
		pos2++;
	if (fileContent[pos2] != '{')
		throw ConfigSyntaxException(); ////// return ?

	size_t	scopeEndIndex = scopeSubStr(fileContent, pos + toFind.size()) + pos2 - pos;
//	std::cout << "pos = " << pos << "\tscopeEndIndex = " << scopeEndIndex << std::endl;
	if (!scopeEndIndex)
		return ("");
	return (fileContent.substr(pos, scopeEndIndex));
//	return (toFind + scope);
}

size_t	Config::scopeSubStr(std::string str, size_t pos) {
	std::string	brackets("{}");
	pos = str.find(brackets[0], pos);
	if (pos == std::string::npos)
		return (0);
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
//	std::cout << "scope: " << str.substr(pos, i) << std::endl;
	return (i);
}

bool	Config::unclosedScope(std::string str, std::string limiter) {
	if (limiter.size() != 2)
		return (false);
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

Config::~Config() {}

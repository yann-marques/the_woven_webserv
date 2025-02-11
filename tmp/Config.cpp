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

void	Config::parseVServRawStr(std::set< std::string > argsToFind, std::map< std::string, std::string >& argsToSet, std::string rawStr) {
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
	do {
		size_t	sepPos1 = newStr.find(':'), sepPos2;
		if (sepPos1 == std::string::npos)
			throw ConfigSyntaxException(); // details ?
		std::string	key(newStr.substr(0, sepPos1));

		// check if key is in reserved words
		std::set< std::string >::iterator	it = argsToFind.find(key), ite = argsToFind.end();
		if (it == ite)
			throw UnexpectedKeyException(key); // unexpected key
		else if (*it == "location") {
			sepPos2 = newStr.find('}');
		} else {
			sepPos2 = newStr.find(';');
		}

		if (sepPos2 == std::string::npos)
			throw ConfigSyntaxException(); // details ?
		std::string	value(newStr.substr(sepPos1 + 1, sepPos2 - sepPos1));

		std::cout
//			<< "newStr = |" << newStr << "|" << std::endl
			<< "key = |" << key << "|" << std::endl
			<< "value = |" << value << "|" << std::endl;
		
		if (argsToSet.count(key))
			throw DoubleArgException(key);
		argsToSet[key] = value;
		newStr.erase(0, sepPos2 + 1);
//		std::cout << "newStr apres erase = |" << newStr << "|" << std::endl; 
	} while (!newStr.empty()); //
}

void	Config::parseVServRawVec() {
//	map< int fd, map< string server_name, Rules* >
//	std::map< int, std::map< std::string, Rules* >

	std::set< std::string >	argsToFind;
	setArgsToFind(argsToFind);

	std::map< std::string, std::string >	argsToSet;

	for (size_t i = 0, rawSize = _vServRawVec.size(); i < rawSize; i++) {
	//	for (size_t j = 0, argSize = argsToFind.size(); i++)
		parseVServRawStr(argsToFind, argsToSet, _vServRawVec[i]); //
		// check args
		checkArgsFormat(argsToSet);

		// get port
		int port = atoi(argsToSet["port"].c_str());
		std::cout << "port: " << port << std::endl;
		// parse location // later
/*		if (argsToSet.count("location")) {
			std::set< std::string >	location = parseLocation(argsToSet["location"])
		}
*/
		// get server_names
		// separate the rest
	//	argsToSet.push_back();
		break ;
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
	return (i);
}

bool	Config::unclosedScope(std::string str, std::string limiter) {
	if (limiter.size() != 2)
		return (false);
	size_t	count = 0, i = 0;
	while (str[i] && count >= 0) {
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

// EXCEPTIONS

const char*	Config::OpenFileException::what() const throw() {
	return ("Cannot open file: ");
}

const char*	Config::UnclosedScopeException::what() const throw() {
	return ("Syntax error: unclosed brackets {}.");
}

const char*	Config::ConfigSyntaxException::what() const throw() {
	return ("Syntax error in configuration file. Run the program with --help for more details.");
}

Config::UnexpectedKeyException::UnexpectedKeyException(std::string where): _str("Unexpected key: " + where) {}

Config::UnexpectedKeyException::~UnexpectedKeyException() throw() {}

const char*	Config::UnexpectedKeyException::what() const throw() {
	return (_str.c_str()); // to complete ?
}

Config::DoubleArgException::DoubleArgException(std::string where): _str("Argument found twice in a single scope: " + where) {}

Config::DoubleArgException::~DoubleArgException() throw() {}

const char*	Config::DoubleArgException::what() const throw() {
	return (_str.c_str());
}

const char*	Config::MissingPortException::what() const throw() {
	return ("Port not found in server scope.");
}

Config::UnexpectedValueException::UnexpectedValueException(std::string where): _str("Unexpected value: " + where) {}

Config::UnexpectedValueException::~UnexpectedValueException() throw() {}

const char*	Config::UnexpectedValueException::what() const throw() {
//	std::string	str("Unexpected value: " + _where);
	return (_str.c_str());
//	return ("Unexpected value"); // to complete ?
}

#include "Config.hpp"

std::string	Config::extractFileContent(const char* fileName) {
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

static bool	bracketsAreClosed(std::string str) {
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

static bool	badSpaces(std::string str) {
	std::string	limiter("{}:,;");
	for (size_t i = 0; str[i]; i++) {
		if (std::isspace(str[i]) && i > 0
			&& limiter.find(str[i - 1]) == std::string::npos) {
			while (str[i] && std::isspace(str[i]))
				i++;
			if (limiter.find(str[i]) == std::string::npos)
				return (true);
		}
	}
	return (false);
}

////
std::vector< std::string >	Config::splitLine(std::string fileContent, std::string sep) {
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
		throw (ArgOutOfServerScopeException());
	return (vec);
}

static bool	isInRange(std::string str, t_mm_range< int, std::string >::t range) {
	t_mm_it< int, std::string >::t	mmIt = range.first, mmIte = range.second;
	while (mmIt != mmIte && mmIt->second != str)
		mmIt++;
	return (mmIt != mmIte);
}

Config::Config(const char* fileName): Parser() {
	std::string	fileContent = extractFileContent(fileName);
	if (!bracketsAreClosed(fileContent))
		throw UnclosedScopeException();
	if (badSpaces(fileContent))
		throw BadSpacesException();

	std::vector< std::string >	serverLines = splitLine(fileContent, "server");
	
}


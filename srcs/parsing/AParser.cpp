/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AParser.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:11:35 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:11:36 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AParser.hpp"

AParser::AParser() {}

AParser::AParser(const AParser& rhs) {
	*this = rhs;
}

AParser&	AParser::operator=(const AParser& rhs) {
	(void) rhs;
	return (*this);
}

size_t	AParser::endOfScopeIndex(std::string str, size_t pos) {
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

std::vector< std::string >	AParser::splitScope(std::string fileContent, std::string sep) {
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

void	AParser::deleteBrackets(std::vector< std::string >&	vec) {
	for (size_t i = 0, n = vec.size(); i < n; i++) {
		vec[i].erase(0, 1);
		vec[i].erase(vec[i].size() - 1, 1);
	}
}

std::multimap< std::string, std::string >	AParser::parseLine(std::string line) {
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


AParser::~AParser() {}
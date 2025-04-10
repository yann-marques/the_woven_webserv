/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AParser_setters.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:11:31 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:11:32 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AParser.hpp"

size_t	AParser::setArgKey(std::string line, std::string& key) {
	size_t	pos = line.find('{');

	if (line.compare(0, pos, "error_pages") && line.compare(0, pos, "cgi_path"))
		pos = line.find(':');
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

	valueLine = line.substr(prvPos + 1, pos - prvPos);

	return (pos);
}

void	AParser::setValues(std::multimap< std::string, std::string >& args, std::string key, std::string valueLine, int scopeCmp) {
	do {
		size_t	pos;
		if (!scopeCmp) {
			pos = valueLine.find(';');
		} else {
			pos = valueLine.find(',');
			if (pos == std::string::npos)
				pos = valueLine.find(';');
		}
		args.insert(make_pair(key, valueLine.substr(0, pos)));
		valueLine.erase(0, pos + 1);
	} while (!valueLine.empty());
}
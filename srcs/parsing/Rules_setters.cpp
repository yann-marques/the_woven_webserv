/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Rules_setters.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:12:22 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:12:22 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Rules.hpp"

void	Rules::setArgsToFind() {
	_argsToFind.insert("location");
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

void	Rules::setAutoIndex(t_mmap_range< std::string, std::string >::t range) {
	t_mmap_it< std::string, std::string >::t it = range.first;
	std::string	str = it != range.second ? it->second : "";
	_autoIndex = (it == range.second
		|| str == "true" || str == "yes" || str == "1");
}

void	Rules::setMaxBodyBytes(t_mmap_range< std::string, std::string >::t range) {
	t_mmap_it< std::string, std::string >::t it = range.first;
	if (it == range.second)
		return ;
	std::string	str = it->second;
	size_t	pos = 0, result = 0;
	while (str[pos] && isdigit(str[pos]))
		pos++;
	result = std::atoi(str.c_str());
	if (str[pos]) {
		if (str[pos] == 'K')
			result *= 1024;
		else if (str[pos] == 'M')
			result *= 1048576;
		else if (str[pos] == 'G')
			result *= 1073741824;
		pos++;
	}
	if (str[pos] && str[pos] != ';')
		result += std::atoi(str.substr(pos).c_str());
	_maxBodyBytes = result;
}

void	Rules::setStrArg(std::string& toSet, t_mmap_range< std::string, std::string >::t range) {
	t_mmap_it< std::string, std::string >::t	it = range.first;
	std::string	str = it != range.second ? it->second : "";
	toSet = str;
}

void	Rules::setErrorPages(t_range range) {
	t_multimap_it	mmIt = range.first, mmIte = range.second;
	while (mmIt != mmIte) {
		std::string	str = mmIt->second;
		size_t	pos1 = str.find(':'), pos2 = str.find(';');
		int	key = std::atoi(str.substr(0, pos1).c_str());
		if (!_errorPages.count(key)) {
			_errorKeys.insert(key);
			_errorPages[key] = str.substr(pos1 + 1, pos2 - pos1 - 1);
		}
		mmIt++;
	}
}

void	Rules::setCgiPath(t_range range) {
	t_multimap_it	mmIt = range.first, mmIte = range.second;
	while (mmIt != mmIte) {
		std::string	str = mmIt->second;
		size_t	pos1 = str.find(':'), pos2 = str.find(';');
		std::string	key = str.substr(0, pos1);
		if (!_cgiPath.count(key)) {
			_cgiKeys.insert(key);
			_cgiPath[key] = str.substr(pos1 + 1, pos2 - pos1 - 1);
		}
		mmIt++;
	}
}

void	Rules::setVector(std::vector< std::string >& vec, t_range range) {
	for (t_multimap_it	mit = range.first, mite = range.second; mit != mite; mit++) {
		std::string	str = mit->second;
		while (!str.empty()) {
			size_t	pos = str.find(',');
			if (pos == std::string::npos)
				pos = str.find(';');
			std::string	subStr = str.substr(0, pos);
			vec.push_back(subStr);
			str.erase(0, subStr.size() + 1);
		}
	}
}

void	Rules::setLocationKey(std::string str) {
	_locationKeys.push_back(str);
}

void	Rules::setArgs(std::multimap< std::string, std::string > args) {
	if (_locationPath != "/") {
		if (args.count("host"))
			throw (RedefinedArgException("host"));
		if (args.count("port"))
			throw (RedefinedArgException("port"));
		if (args.count("server_names"))
			throw (RedefinedArgException("server_names"));
	}

	setAutoIndex(args.equal_range("auto_index"));
	setMaxBodyBytes(args.equal_range("max_body_bytes"));
	setStrArg(_root, args.equal_range("root"));
	setStrArg(_redirect, args.equal_range("redirect"));
	setStrArg(_upload, args.equal_range("upload"));

	setVector(_defaultPages, args.equal_range("default_pages"));
	setVector(_allowedMethods, args.equal_range("allowed_methods"));

	setErrorPages(args.equal_range("error_pages"));
	setCgiPath(args.equal_range("cgi_path"));
}

void	Rules::setLocation(t_range range) {
	t_multimap_it	mit = range.first, mite = range.second;
	while (mit != mite) {
		std::multimap< std::string, std::string >	args = parseLocationLine(mit->second);
		checkArgsFormat(args);

		if (!_locationKeys.size())
			return ;
		for (size_t i = 0, n = _locationKeys.size(); i < n; i++) {
			if (!_location.count(_locationKeys[i])) {
				std::string	newLocPath;
				if (_locationPath == "/")
					newLocPath = _locationKeys[i];
				else
					newLocPath = _locationPath + _locationKeys[i];
				_location[_locationKeys[i]] = new Rules(args, *this, newLocPath);
			}
		}
		args.clear();
		mit++;
	}
}
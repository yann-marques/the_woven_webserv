/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Rules.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:12:26 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:12:26 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Rules.hpp"

Rules::Rules(): AParser() {
	_autoIndex = true;
	_maxBodyBytes = 1024;
	_root = "www/";

	_defaultPages.push_back("index");
	_defaultPages.push_back("index.html");

	_allowedMethods.push_back("GET");
	_allowedMethods.push_back("HEAD");
}

Rules::Rules(std::multimap< std::string, std::string > args, const Rules& rhs, std::string locationPath): AParser() {
	try {
		setArgsToFind();
		_args = args;
		_locationPath = locationPath;
		setArgs(_args);
		*this |= rhs;
		if (args.count("location"))
			setLocation(args.equal_range("location"));
	} catch (AParser::ArgOutOfServerScopeException& e) {
		destruct();
		throw(e);
	} catch (AParser::ConfigSyntaxException& e) {
		destruct();
		throw(e);
	} catch (AParser::UnexpectedKeyException& e) {
		destruct();
		throw(e);
	} catch (AParser::UnexpectedValueException& e) {
		destruct();
		throw(e);
	} catch (AParser::DoubleArgException& e) {
		destruct();
		throw(e);
	} catch (AParser::ForbiddenCharException& e) {
		destruct();
		throw(e);
	} catch (Rules::RedefinedArgException& e) {
		destruct();
		throw(e);
	} catch (Rules::InvalidLocationKeyException& e) {
		destruct();
		throw(e);
	} catch (std::exception& e) {
		destruct();
		throw(e);
	}
}


Rules::Rules(const Rules& rhs): AParser() {
	*this = rhs;
}

Rules&	Rules::operator=(const Rules& rhs) {
	_args = rhs.getArgs();
	_locationPath = rhs.getLocationPath();
	_autoIndex = rhs.getAutoIndex();
	_maxBodyBytes = rhs.getMaxBodyBytes();
	_root = rhs.getRoot();
	_redirect = rhs.getRedirect();
	_upload = rhs.getUpload();
	_defaultPages = rhs.getDefaultPages();
	_allowedMethods = rhs.getAllowedMethods();
	_errorKeys = rhs.getErrorKeys();
	_errorPages = rhs.getErrorPages();
	_cgiKeys = rhs.getCgiKeys();
	_cgiPath = rhs.getCgiPath();

	_locationKeys = rhs.getLocationKeys();
	for (std::vector< std::string >::iterator it = _locationKeys.begin(), ite = _locationKeys.end(); it != ite; it++) {
		std::string	key = *it;
		_location[key] = new Rules(*(rhs.getLocation().at(key)));
	}
	return (*this);
}

Rules&	Rules::operator|=(const Rules& rhs) {
	if (!_args.count("root"))
		_root = rhs.getRoot();
	if (!_args.count("default_pages"))
		_defaultPages = rhs.getDefaultPages();
	if (!_args.count("auto_index"))
		_autoIndex = rhs.getAutoIndex();
	if (!_args.count("allowed_methods"))
		_allowedMethods = rhs.getAllowedMethods();
	if (!_args.count("max_body_bytes"))
		_maxBodyBytes = rhs.getMaxBodyBytes();
	if (!_args.count("redirect"))
		_redirect = rhs.getRedirect();
	if (!_args.count("upload"))
		_upload = rhs.getUpload();

	if (!_args.count("error_pages")) {
		_errorKeys = rhs.getErrorKeys();
		_errorPages = rhs.getErrorPages();
	}
	if (!_args.count("cgi_path")) {
		_cgiKeys = rhs.getCgiKeys();
		_cgiPath = rhs.getCgiPath();
	}
	return (*this);
}
std::multimap< std::string, std::string >	Rules::parseLocationLine(std::string line) {
	std::multimap< std::string, std::string >	args;
	std::string	key;
	size_t	pos = line.find(',');
	while (pos != std::string::npos && pos < line.find('{')) {
		key = line.substr(0, pos);
		if (!isValidLocationKey(key))
			throw (InvalidLocationKeyException(key));
		setLocationKey(key);
		pos++;
		line.erase(0, pos);
		pos = line.rfind(',');
	}
	pos = line.find('{');
	if (pos != std::string::npos) {
		key = line.substr(0, pos);
		if (!isValidLocationKey(key))
			throw (InvalidLocationKeyException(key));
		setLocationKey(key);
		pos++;
		line.erase(0, pos);
		pos = line.rfind('}');
		if (pos != std::string::npos)
			line.erase(pos, 1);
	}
	args = parseLine(line);
	return (args);
}

void	Rules::printDeep(size_t i, std::string name) {
	std::string	slashes(i * 8, '/');
	std::string	tabs(i, '\t');
	std::cout	<< slashes << " PRINTDEEP " << i << " : " << name << " " << this << std::endl
				<< tabs << "locationPath:\t" << getLocationPath() << std::endl
				<< tabs << "root:\t" << getRoot() << std::endl
				<< tabs << "autoIndex:\t" << getAutoIndex() << std::endl
				<< tabs << "maxBodyBytes:\t" << getMaxBodyBytes() << std::endl
				<< tabs << "redirect:\t" << getRedirect() << std::endl
				<< tabs << "upload:\t" << getUpload() << std::endl
				<< tabs << "defaultPages:" << std::endl;
	printVec(getDefaultPages(), tabs);
	std::cout << tabs << "allowedMethods:" << std::endl;
	printVec(getAllowedMethods(), tabs);
		
	std::cout << tabs << "cgiPaths:" << std::endl;
	printMap(getCgiKeys(), getCgiPath(), tabs);
	std::cout << tabs << "errorPages:" << std::endl;
	printMap(getErrorKeys(), getErrorPages(), tabs);
	std::cout << std::endl;

	for (size_t pos = 0, n = _locationKeys.size(); pos < n; pos++) {
		std::cout	<< tabs << name << " contains " << _locationKeys[pos]
					<< " " << _location[_locationKeys[pos]] << std::endl;
		_location[_locationKeys[pos]]->printDeep(i + 1, _locationKeys[pos]);
	}
}

void	Rules::destruct() {
	if (!_location.size())
		return ;

	for (size_t i = 0, n = _locationKeys.size(); i < n; i++)
		delete _location[_locationKeys[i]];

	_defaultPages.clear();
	_errorPages.clear();
	_allowedMethods.clear();
	_cgiPath.clear();
	_locationKeys.clear();
	_location.clear();
}

Rules::~Rules() {
	destruct();
}

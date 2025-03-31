/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Rules_getters.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:12:17 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:12:18 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Rules.hpp"

const std::multimap< std::string, std::string >&	Rules::getArgs() const {
	return (_args);
}

const std::string&	Rules::getLocationPath() const {
	return (_locationPath);
}

const std::string&	Rules::getRoot() const {
	return (_root);
}

const std::vector< std::string >&	Rules::getDefaultPages() const {
	return (_defaultPages);
}

const std::set< int >&	Rules::getErrorKeys() const {
	return (_errorKeys);
}

const std::map< int, std::string >&	Rules::getErrorPages() const {
	return (_errorPages);
}

const bool&	Rules::getAutoIndex() const {
	return (_autoIndex);
}

const std::vector< std::string >&	Rules::getAllowedMethods() const {
	return (_allowedMethods);
}

const size_t&	Rules::getMaxBodyBytes() const {
	return (_maxBodyBytes);
}

const std::set< std::string >&	Rules::getCgiKeys() const {
	return (_cgiKeys);
}

const std::map< std::string, std::string >&	Rules::getCgiPath() const {
	return (_cgiPath);
}

const std::string&	Rules::getRedirect() const {
	return (_redirect);
}

const std::string&	Rules::getUpload() const {
	return (_upload);
}

const std::vector< std::string >&	Rules::getLocationKeys() const {
	return (_locationKeys);
}

const std::map< std::string, Rules* >&	Rules::getLocation() const {
	return (_location);
}

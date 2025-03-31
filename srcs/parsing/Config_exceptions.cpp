/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config_exceptions.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:11:48 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:22:25 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::IsDirException::IsDirException(std::string where):
	StrException(where + " is a directory") {}

Config::IsDirException::~IsDirException() throw() {}

const char*	Config::IsDirException::what() const throw() {
	return (_str.c_str());
}

Config::OpenFileException::OpenFileException(std::string where):
	StrException("Cannot open file : " + where) {}

Config::OpenFileException::~OpenFileException() throw() {}

const char*	Config::OpenFileException::what() const throw() {
	return (_str.c_str());
}

const char*	Config::UnclosedScopeException::what() const throw() {
	return ("Syntax error: unclosed brackets {}");
}

const char*	Config::BadSpacesException::what() const throw() {
	return ("Syntax error: words must be separated by , : ; { }");
}

const char*	Config::MissingPortException::what() const throw() {
	return ("Port not found in server scope.");
}

Config::MultipleDefinitionOfPort::MultipleDefinitionOfPort(std::string where):
	StrException("Port used multiple times for the same host is not allowed: " + where) {}

	
Config::MultipleDefinitionOfPort::~MultipleDefinitionOfPort() throw() {}

const char*	Config::MultipleDefinitionOfPort::what() const throw() {
	return (_str.c_str());
}

const char*	Config::EmptyFileException::what() const throw() {
	return ("File is empty.");
}
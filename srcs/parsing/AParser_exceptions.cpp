/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AParser_exceptions.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: locharve <locharve@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:11:27 by locharve          #+#    #+#             */
/*   Updated: 2025/03/31 14:11:28 by locharve         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AParser.hpp"

const char*	AParser::ArgOutOfServerScopeException::what() const throw() {
	return ("Argument found out of server scopes");
}

const char*	AParser::ConfigSyntaxException::what() const throw() {
	return ("Syntax error in configuration file");
}

AParser::UnexpectedKeyException::UnexpectedKeyException(std::string where):
	StrException("Unexpected key: " + where) {}

AParser::UnexpectedKeyException::~UnexpectedKeyException() throw() {}

const char*	AParser::UnexpectedKeyException::what() const throw() {
	return (_str.c_str());
}

AParser::UnexpectedValueException::UnexpectedValueException(std::string where):
	StrException("Unexpected value: " + where) {}

AParser::UnexpectedValueException::~UnexpectedValueException() throw() {}

const char*	AParser::UnexpectedValueException::what() const throw() {
	return (_str.c_str());
}

AParser::DoubleArgException::DoubleArgException(std::string where):
	StrException("Argument found twice in a single scope: " + where) {}

AParser::DoubleArgException::~DoubleArgException() throw() {}

const char*	AParser::DoubleArgException::what() const throw() {
	return (_str.c_str());
}

AParser::ForbiddenCharException::ForbiddenCharException(std::string where):
	StrException("Forbidden char in argument: " + where) {}

AParser::ForbiddenCharException::~ForbiddenCharException() throw() {}

const char*	AParser::ForbiddenCharException::what() const throw() {
	return (_str.c_str());
}

const char*	AParser::EmptyStrException::what() const throw() {
	return ("Empty string as value is not allowed.");
}
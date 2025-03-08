#include "Parser.hpp"

const char*	Parser::ArgOutOfServerScopeException::what() const throw() {
	return ("Argument found out of server scopes.");
}

const char*	Parser::ConfigSyntaxException::what() const throw() {
	return ("Syntax error in configuration file. Run the program with --help for more details.");
}

Parser::UnexpectedKeyException::UnexpectedKeyException(std::string where): StrException("Unexpected key: " + where) {}

Parser::UnexpectedKeyException::~UnexpectedKeyException() throw() {}

const char*	Parser::UnexpectedKeyException::what() const throw() {
	return (_str.c_str());
}

Parser::UnexpectedValueException::UnexpectedValueException(std::string where): StrException("Unexpected value: " + where) {}

Parser::UnexpectedValueException::~UnexpectedValueException() throw() {}

const char*	Parser::UnexpectedValueException::what() const throw() {
	return (_str.c_str());
}

Parser::DoubleArgException::DoubleArgException(std::string where): StrException("Argument found twice in a single scope: " + where) {}

Parser::DoubleArgException::~DoubleArgException() throw() {}

const char*	Parser::DoubleArgException::what() const throw() {
	return (_str.c_str());
}
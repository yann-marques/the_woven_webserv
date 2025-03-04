#include "parsing/Config.hpp"

const char*	Config::OpenFileException::what() const throw() {
	return ("Cannot open file: ");
}

const char*	Config::ArgOutsideServerScopeException::what() const throw() {
	return ("Argument found outside server scopes.");
}

const char*	Config::UnclosedScopeException::what() const throw() {
	return ("Syntax error: unclosed brackets {}.");
}

const char*	Config::ConfigSyntaxException::what() const throw() {
	return ("Syntax error in configuration file. Run the program with --help for more details.");
}

Config::UnexpectedKeyException::UnexpectedKeyException(std::string where): StrException("Unexpected key: " + where) {}

Config::UnexpectedKeyException::~UnexpectedKeyException() throw() {}

const char*	Config::UnexpectedKeyException::what() const throw() {
	return (_str.c_str());
}

Config::DoubleArgException::DoubleArgException(std::string where): StrException("Argument found twice in a single scope: " + where) {}

Config::DoubleArgException::~DoubleArgException() throw() {}

const char*	Config::DoubleArgException::what() const throw() {
	return (_str.c_str());
}

const char*	Config::MissingPortException::what() const throw() {
	return ("Port not found in server scope.");
}

const char*	Config::MultiplePortsException::what() const throw() {
	return ("Multiple ports defined in server scope.");
}

Config::UnexpectedValueException::UnexpectedValueException(std::string where): StrException("Unexpected value: " + where) {}

Config::UnexpectedValueException::~UnexpectedValueException() throw() {}

const char*	Config::UnexpectedValueException::what() const throw() {
	return (_str.c_str());
}
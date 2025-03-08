#include "Config.hpp"

const char*	Config::OpenFileException::what() const throw() {
	return ("Cannot open file: ");
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

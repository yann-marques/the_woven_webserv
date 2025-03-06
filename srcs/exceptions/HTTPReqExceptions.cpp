#include "HTTPRequest.hpp"

const char*	HttpRequest::OpenFileException::what() const throw() {
	return ("Error to opening the file");
}

const char*	HttpRequest::MalformedHttpHeader::what() const throw() {
	return ("Malformed http header");
}
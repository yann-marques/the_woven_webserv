#include "HTTPRequest.hpp"

const char*	HttpRequest::OpenFileException::what() const throw() {
	return ("Error to opening the file");
}
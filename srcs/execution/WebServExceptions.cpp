#include "WebServ.hpp"

const char*	WebServ::SIGINTException::what() const throw() {
	return ("Signal received: interrupting the server.");
}

const char*	WebServ::EpollCreateException::what() const throw() {
	return ("Failed to create epoll instance.");
}

const char*	WebServ::EpollCtlAddException::what() const throw() {
	return ("Failed to add fd to epoll instance.");
}

const char*	WebServ::EpollCtlDelException::what() const throw() {
	return ("Failed to remove fd to epoll instance.");
}

const char*	WebServ::EpollWaitException::what() const throw() {
	return ("Failed to create epoll wait the events.");
}

const char*	WebServ::UnknownFdException::what() const throw() {
	return ("The fd is neither for a client and server");
}
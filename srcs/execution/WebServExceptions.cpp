#include "WebServ.hpp"

WebServ::SignalException::SignalException(int signal) {
	std::string	sigStr = signal == SIGINT ? "SIGINT" : SIGQUIT ? "SIGQUIT" : "";
	_str = (sigStr + " received: interrupting the server.");
}

WebServ::SignalException::~SignalException() throw() {}

const char*	WebServ::SignalException::what() const throw() {
	return (_str.c_str());
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
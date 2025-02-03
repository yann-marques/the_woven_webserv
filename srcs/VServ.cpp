#include "VServ.hpp"

// VServ::VServ();

VServ::VServ(VServConfig config, int maxClients): _maxClients(maxClients) {
	// tmp
	_port = config.getPort();
	_host = config.getHost();
	// parse config ...
	setAddress();
	socketInit();

	setEvent();
}

// VServ::VServ(const VServ& rhs);

VServ&	VServ::operator=(const VServ& rhs) {
	_port = rhs.getPort();
	_host = rhs.getHost();
	_fd = rhs.getFd();
	setAddress();
	setEvent();
	return (*this);
}

VServ::~VServ() {
//	if (_fd != -1)
//		close(_fd);
}

// SETTERS

void	VServ::setAddress() {
	_address.sin_family = AF_INET;
	_address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_port = htons(_port);
}

void	VServ::setEvent() {
	_event.events = EPOLLIN;
	_event.data.fd = _fd;
}

// GETTERS

int	VServ::getPort() const {
	return (_port);
}

int	VServ::getHost() const {
	return (_host);
}

int	VServ::getFd() const {
	return (_fd);
}

//

void	VServ::socketInit() {
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == -1)
		throw (SocketException());
	fcntl(_fd, F_SETFL, O_NONBLOCK); // setNonBlocking

	int opt = 1; // member attribute ? need later ?
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		throw (SetSockOptException());

	if (bind(_fd, (struct sockaddr*)&_address, sizeof(_address)) == -1)
		throw (BindException());

	if (listen(_fd, _maxClients) == -1)
		throw (ListenException());

	// catch in WebServ constructor
}

void	VServ::epollCtl(int epollFd) {
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, _fd, &_event) == -1)
		throw (EpollCtlException());
}

const char*	VServ::SocketException::what() const throw() {
	return ("Failed to create socket.");
}

const char*	VServ::SetSockOptException::what() const throw() {
	return ("Failed to set socket opt.");
}

const char*	VServ::BindException::what() const throw() {
	return ("Failed to bind socket.");
}

const char*	VServ::ListenException::what() const throw() {
	return ("Failed to listen.");
}

const char*	VServ::EpollCtlException::what() const throw() {
	return ("Failed to add server socket to epoll instance.");
}

std::ostream&	operator<<(std::ostream& os, const VServ& vs) {
	os	<< "----------- VSERV -----------" << std::endl
		<< "\tport:\t" << vs.getPort() << std::endl
		<< "\thost:\t" << vs.getHost() << std::endl
		<< "\tfd:\t" << vs.getFd() << std::endl;
	// ...
	return (os);
}
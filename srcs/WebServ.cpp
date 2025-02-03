#include "WebServ.hpp"

// WebServ::WebServ() {} // private ?

WebServ::WebServ(std::string filename): _config(filename) {
//	signal(SIGINT, handleSignal); // in execution

	try {
		// epoll init
		_epollFd = epoll_create(_maxClients + 1);
		if (_epollFd == -1)
			throw EpollCreateException();

		_serverNbr = _config.getServersNbr();
		std::cout << "serverNbr = " << _serverNbr << std::endl;
		for (size_t i = 0; i < _serverNbr; i++) { // c.serverConfig : container ? vector
			VServ*	server = new VServ(_config.getServerConfig(i), _maxClients); // epoll ? epollFd en arg ?
			int	sfd = server->getFd();

			setServerFd(sfd);
			setServer(sfd, server);

			// epoll ctl in VServ
			_servers[sfd]->epollCtl(_epollFd);
		}
	} catch (EpollCreateException& e) {
		std::cerr << e.what() << std::endl;
	} catch (VServ::SocketException& e) {
		std::cerr << e.what() << std::endl;
	} catch (VServ::SetSockOptException& e) {
		std::cerr << e.what() << std::endl;
	} catch (VServ::BindException& e) {
		std::cerr << e.what() << std::endl;
	} catch (VServ::ListenException& e) {
		std::cerr << e.what() << std::endl;
	} catch (VServ::EpollCtlException& e) {
		std::cerr << e.what() << std::endl;
	}
}

// WebServ::WebServ(const Webserv& rhs) {} // private ?

// WebServ&	WebServ::operator=(const WebServ& rhs) {} // private ?

WebServ::~WebServ() {
	// in ~VServ()
/*
	int	size = _serverFds.size();
	for (int i = 0; i < size; i++)
		if (_serverFds[i] != -1)
			close(_serverFds[i]);
*/
	if (_epollFd != -1)
		close(_epollFd);
	size_t	size = getServerNbr();
	for (size_t i = 0; i < size; i++)
		delete getServer(getServerFd(i));
/*
	size = _clientFds.size();

	for (int i = 0; i < size; i++)
		close(_clientFds[i]);
*/
}

// SETTERS

void	WebServ::setServerFd(int fd) {
	_serverFds.push_back(fd);
}

void	WebServ::setServer(int fd, VServ* rhs) {
	_servers[fd] = rhs;
}

void	WebServ::setClientFd(int i, int fd) {
	_clientFds[i] = fd;
}

// GETTERS

int	WebServ::getEpollFd() const {
	return (_epollFd);
}

int	WebServ::getServerFd(int i) const {
	return (_serverFds[i]);
}

VServ*	WebServ::getServer(int fd) {
	return (_servers[fd]);
}

int	WebServ::getClientFd(int i) const {
	return (_clientFds[i]);
}

std::size_t	WebServ::getServerNbr() const {
	return (_serverNbr);
}

//

void	WebServ::handleSignal(int signal) {
	if (signal == SIGINT)
		throw (SIGINTException());
}

// EXCEPTIONS

const char*	WebServ::SIGINTException::what() const throw() {
	return ("Signal received: interrupting the server.");
}

const char*	WebServ::EpollCreateException::what() const throw() {
	return ("Failed to create epoll instance.");
}

std::ostream&	operator<<(std::ostream& os, WebServ& ws) {
	size_t	size = ws.getServerNbr();

	os	<< "////////// WEBSERV //////////" << std::endl
		<< "\tepollFd = " << ws.getEpollFd() << std::endl;

	for (size_t i = 0; i < size; i++) {
		os << *(ws.getServer(ws.getServerFd(i)));
	}
	return (os);
}
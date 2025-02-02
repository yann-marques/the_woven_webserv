#include "WebServ.hpp"

// WebServ::WebServ() {} // private ?

WebServ::WebServ(std::string filename): _config(filename) {
//	signal(SIGINT, handleSignal); // in execution

	try {
		// epoll init
		_epollFd = epoll_create(_maxClients + 1);
		if (_epollFd == -1)
			throw EpollCreateException();

		int	serversNbr = _config.serverConfig.size();
		for (int i = 0; i < serversNbr; i++) { // c.serverConfig : container ? vector
			VServ	server(_config.serverConfig[i]); // epoll ? epollFd en arg ?
			int	sfd = server.getFd();

			setServerFd(i, sfd);
			setServer(sfd, server);

			// epoll ctl in VServ
			_servers[sfd].epollCtl(_epollFd);
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
/*
	size = _clientFds.size();

	for (int i = 0; i < size; i++)
		close(_clientFds[i]);
*/
}

// SETTERS

void	setServerFd(int i, int fd) {
	_serverFds[i] = fd;
}

void	setServer(int fd, const VServ& rhs) {
	_servers[fd] = rhs;
}

void	setClientFd(int i, int fd) {
	_clientFds[i] = fd;
}

// GETTERS

int	WebServ::getServerFd(int i) const {
	return (_serverFds[i]);
}

VServ&	WebServ::getVServ(int fd) const {
	return (_servers[fd]);
}

int	WebServ::getClientFd(int i) const {
	return (_clientFds[i]);
}

//

void	WebServ::handleSignal(int signal) {
	if (signal == SIGINT)
		throw (StopSignalException());
}

// EXCEPTIONS

const char*	WebServ::SIGINTException::what() const throw() {
	return ("Signal received: interrupting the server.");
}

const char*	WebServ::EpollCreateException::what() const throw() {
	return ("Failed to create epoll instance.");
}
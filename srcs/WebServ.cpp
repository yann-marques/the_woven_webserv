#include "WebServ.hpp"

WebServ::WebServ() {} // private ?

WebServ::WebServ(std::string filename) {
	_config.parse(filename);

	int	serversNbr = _config.serverConfig.size();

	for (int i = 0; i < serversNbr; i++) { // c.serverConfig : container ? vector
		int	sfd = _config.serverConfig[i].fd;
		VServ	server(_config.serverConfig[i]);

		setServerFd(i, sfd);
		setServer(sfd, server);
		//
	}

}

WebServ::WebServ(const Webserv& rhs) {} // private ?

WebServ&	WebServ::operator=(const WebServ& rhs) {} // private ?

WebServ::~WebServ() {
	int	size = _serverFds.size();

	for (int i = 0; i < size; i++)
		close(_serverFds[i]);
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

void	setServer(int fd, const VServ& rhs) { // copy operator VServ ?
	// if VServ pointers
	// VServ*	server = new VServ(rhs);
	_servers[fd] = rhs;
}

void	setClientFd(int i, int fd) {
	_clientFds[i] = fd;
}

// GETTERS

int	WebServ::getServerFd(int i) {
	return (_serverFds[i]);
}

VServ	WebServ::getVServ(int fd) {
	return (_servers[fd]);
}

int	WebServ::getClientFd(int i) {
	return (_clientFds[i]);
}